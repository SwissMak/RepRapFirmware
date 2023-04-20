/*
 * Spindle.cpp
 *
 *  Created on: Mar 21, 2018
 *      Author: Christian
 */

#include "Spindle.h"
#include <Platform/RepRap.h>
#include <GCodes/GCodeBuffer/GCodeBuffer.h>
#include <Platform/Platform.h>

#if SUPPORT_OBJECT_MODEL

// Object model table and functions
// Note: if using GCC version 7.3.1 20180622 and lambda functions are used in this table, you must compile this file with option -std=gnu++17.
// Otherwise the table will be allocated in RAM instead of flash, which wastes too much RAM.

// Macro to build a standard lambda function that includes the necessary type conversions
#define OBJECT_MODEL_FUNC(...) OBJECT_MODEL_FUNC_BODY(Spindle, __VA_ARGS__)
#define OBJECT_MODEL_FUNC_IF(...) OBJECT_MODEL_FUNC_IF_BODY(Spindle, __VA_ARGS__)

constexpr ObjectModelTableEntry Spindle::objectModelTable[] =
{
	// Within each group, these entries must be in alphabetical order
	// 0. Spindle members
	//{ "active",			OBJECT_MODEL_FUNC((int32_t)self->configuredRpm),ObjectModelEntryFlags::none },
	//{ "current",		OBJECT_MODEL_FUNC((int32_t)self->currentRpm),	ObjectModelEntryFlags::live },
	//{ "frequency",		OBJECT_MODEL_FUNC((int32_t)self->frequency),	ObjectModelEntryFlags::verbose },
	//{ "max",			OBJECT_MODEL_FUNC((int32_t)self->maxRpm),		ObjectModelEntryFlags::verbose },
	//{ "min",			OBJECT_MODEL_FUNC((int32_t)self->minRpm),		ObjectModelEntryFlags::verbose },
	//{ "state",			OBJECT_MODEL_FUNC(self->state.ToString()),		ObjectModelEntryFlags::live },
    { "t1",			OBJECT_MODEL_FUNC(self->currentRpm),	ObjectModelEntryFlags::live },
	{ "t2",			OBJECT_MODEL_FUNC(self->configuredRpm,1),	ObjectModelEntryFlags::live },
	{ "t3",			OBJECT_MODEL_FUNC(self->currentPosition,2),	ObjectModelEntryFlags::live },
	{ "t4",			OBJECT_MODEL_FUNC(self->configuredPosition,2),	ObjectModelEntryFlags::live },
	{ "t5",			OBJECT_MODEL_FUNC(self->currentTorque,2),	ObjectModelEntryFlags::live },
	{ "t6",			OBJECT_MODEL_FUNC(self->currentLoading,2),	ObjectModelEntryFlags::live },
};

constexpr uint8_t Spindle::objectModelTableDescriptor[] = { 1, 6 };

DEFINE_GET_OBJECT_MODEL_TABLE(Spindle)

#endif

Spindle::Spindle() noexcept
	: currentRpm(0),
	  configuredRpm(0),
	  minRpm(DefaultMinSpindleRpm),
	  maxRpm(DefaultMaxSpindleRpm),
	  frequency(0),
	  state(SpindleState::unconfigured)
{
}

GCodeResult Spindle::Configure(GCodeBuffer& gb, const StringRef& reply) THROWS(GCodeException)
{
	bool seen = false;
	if (gb.Seen('C'))
	{
		seen = true;
		IoPort * const ports[] = { &pwmPort, &onOffPort, &reverseNotForwardPort };
		const PinAccess access[] = { PinAccess::pwm, PinAccess::write0, PinAccess::write0 };
		if (IoPort::AssignPorts(gb, reply, PinUsedBy::spindle, 3, ports, access) == 0)
		{
			return GCodeResult::error;
		}
	}

	if (gb.Seen('Q'))
	{
		seen = true;
		frequency = gb.GetPwmFrequency();
		pwmPort.SetFrequency(frequency);
	}

	if (gb.Seen('L'))
	{
		seen = true;
		uint32_t rpm[2];
		size_t numValues = 2;
		gb.GetUnsignedArray(rpm, numValues, false);
		if (numValues == 2)
		{
			minRpm = rpm[0];
			maxRpm = rpm[1];
		}
		else
		{
			minRpm = DefaultMinSpindleRpm;
			maxRpm = rpm[0];
		}
	}

	if (seen)
	{
		state = SpindleState::stopped;
		reprap.SpindlesUpdated();
	}
	return GCodeResult::ok;
}

void Spindle::SetConfiguredRpm(const uint32_t rpm, bool updateCurrentRpm) noexcept
{
	configuredRpm = rpm;
	if (updateCurrentRpm)
	{
		SetRpm(configuredRpm);
	}
	reprap.SpindlesUpdated();			// configuredRpm is not flagged live
}

void Spindle::SetRpm(uint32_t rpm) noexcept
{
	if (state == SpindleState::stopped || rpm == 0)
	{
		onOffPort.WriteDigital(false);
		pwmPort.WriteAnalog(0.0);
		currentRpm = 0;						// current rpm is flagged live, so no need to change seqs.spindles
	}
	else if (state == SpindleState::forward)
	{
		rpm = constrain<int>(rpm, minRpm, maxRpm);
		reverseNotForwardPort.WriteDigital(false);
		pwmPort.WriteAnalog((float)(rpm - minRpm) / (float)(maxRpm - minRpm));
		onOffPort.WriteDigital(true);
		currentRpm = rpm;					// current rpm is flagged live, so no need to change seqs.spindles
	}
	else if (state == SpindleState::reverse)
	{
		rpm = constrain<int>(-rpm, -maxRpm, -minRpm);
		reverseNotForwardPort.WriteDigital(true);
		pwmPort.WriteAnalog((float)(-rpm - minRpm) / (float)(maxRpm - minRpm));
		onOffPort.WriteDigital(true);
		currentRpm = -rpm;					// current rpm is flagged live, so no need to change seqs.spindles
	}
}

void Spindle::SetState(const SpindleState newState) noexcept {
	if (state != newState)
	{
		state = newState;
		SetRpm(configuredRpm);				// Depending on the configured SpindleState this might actually stop the spindle
	}
}

void Spindle::TurnOff() noexcept
{
	spindleReversePort.WriteAnalog(0.0);
	spindleForwardPort.WriteAnalog(0.0);
	currentRpm = 0.0;
}
void Spindle::SetCurrentRpm(float _value) noexcept
{
	if(DriverType == 0){
		_value = (_value * 60 * Direction) / GearRatio;
	}else{
		_value = (_value * Direction) / GearRatio;
	}
	currentRpm = _value;
}
void Spindle::SetCurrentPosition(float _value) noexcept
{
	if(DriverType == 0){
		_value = (_value * 360 * Direction) / GearRatio;
	}else{
		_value = (_value * Direction) / GearRatio;
	}
	_value -= _offset;
	_value *= 100;
	_value = (float)((int)_value % 36000);
	_value /= 100;
	currentPosition = _value;
}
void Spindle::SetcurrentTorque(float _value) noexcept
{
	if(DriverType == 0){
		_value = (_value * 360 * Direction) / GearRatio;
	}else{
		_value = (_value * Direction) / GearRatio;
	}
	currentTorque = _value;
}
void Spindle::SetOdriveParameter(const char* Mcode, const char* Axis) noexcept
{
	Mcode_for_Odrive_S.copy(Mcode);
	Axis_for_Odrive_S.copy(Axis);
}
void Spindle::GetIdle(const StringRef& response_str) noexcept
{
	if(DriverType == 0){
		//"M840 A(w axis0.requested_state 1)\n"
		response_str.copy(Mcode_for_Odrive_S.c_str());
		response_str.catf(" B(w axis%s.requested_state 1)\n", Axis_for_Odrive_S.c_str());
	}else{
		//":C 030602140101\n"
		response_str.copy("");
		response_str.catf(":C 0%d0602140101\n", DeltaIndex );
	}
}
void Spindle::GetVelocity(const StringRef& response_str , int _value) noexcept
{//01745329
	if(_Lock){
		GetIdle(response_str);
	}
	else if(DriverType == 0){
		_value = round(_value* 0.016666) * GearRatio * Direction;
		response_str.copy(Mcode_for_Odrive_S.c_str());
		response_str.catf(" B(v %s %d)\n", Axis_for_Odrive_S.c_str() , _value);
	}else{
		// ":W 03 V %08X\n"
		_value = _value * GearRatio * Direction * 10;
		_value = (_value << 16) | ((_value >> 16) & 0xFFFF);
		response_str.copy("");
		response_str.catf(":W 0%d V %08X\n", DeltaIndex, _value);
	}
}
void Spindle::GetTrap_traj_vel_limit(const StringRef& response_str , float _value) noexcept
{
	if(DriverType == 0){
		response_str.copy(Mcode_for_Odrive_S.c_str());
		response_str.catf(" B(w axis%s.trap_traj.config.vel_limit %f)\n", Axis_for_Odrive_S.c_str() , (double)_value);
	}else response_str.copy("");
}
void Spindle::GetTrap_traj_accel_limit(const StringRef& response_str , float _value) noexcept
{
	if(DriverType == 0){
		response_str.copy(Mcode_for_Odrive_S.c_str());
		response_str.catf(" B(w axis%s.trap_traj.config.accel_limit %f)\n", Axis_for_Odrive_S.c_str() , (double)_value);
	}else response_str.copy("");
}
void Spindle::GetTrap_traj_decel_limit(const StringRef& response_str , float _value) noexcept
{
	if(DriverType == 0){
		response_str.copy(Mcode_for_Odrive_S.c_str());
		response_str.catf(" B(w axis%s.trap_traj.config.decel_limit %f)\n", Axis_for_Odrive_S.c_str() , (double)_value);
	}else response_str.copy("");
}
void Spindle::GetPosition(const StringRef& response_str , float _value) noexcept
{
	if(_Lock){
		GetIdle(response_str);
	}
	else if(DriverType == 0){
		GetIdle(response_str);
		response_str.catf( "%s" , Mcode_for_Odrive_S.c_str());
		response_str.catf(" B(esl %s 0)\n", Axis_for_Odrive_S.c_str());
		//
		response_str.catf("%s B(W axis%s.requested_state 8)\n", Mcode_for_Odrive_S.c_str() , Axis_for_Odrive_S.c_str());
		//
		_value += _offset;
		_value = _value * 0.002777 * GearRatio * Direction;
		response_str.catf("%s B(t %s %f)\n", Mcode_for_Odrive_S.c_str() , Axis_for_Odrive_S.c_str() , (double)_value);
	}else{
		//":W 02 P %08X\n"
		_value += _offset;
		_value = _value * GearRatio * Direction * PUU / 360;
		int _iValue = (int)_value;
		_iValue = (_iValue << 16) | ((_iValue >> 16) & 0xFFFF);
		response_str.catf(":W 0%d P %08X\n", DeltaIndex, _iValue );
	}
}
bool Spindle::GetIsReachRPM() noexcept
{
	//1% gap
	if( abs(configuredRpm * 0.01) <= abs(configuredRpm - currentRpm)){
		return false;
	}else return true;
}
void Spindle::GetReachRPMstring(const StringRef& response_str) noexcept
{
	response_str.catf( "%d %d %d %d" , (double)configuredRpm, (double)currentRpm, (double)abs(configuredRpm * 0.01), (double)abs(configuredRpm - currentRpm)   );
}
// End
