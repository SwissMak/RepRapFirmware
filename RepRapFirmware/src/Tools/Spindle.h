/*
 * Spindle.h
 *
 *  Created on: Mar 21, 2018
 *      Author: Christian
 */

#ifndef SPINDLE_H
#define SPINDLE_H

#include <RepRapFirmware.h>
#include <Hardware/IoPorts.h>
#include <ObjectModel/ObjectModel.h>
#include <General/NamedEnum.h>

NamedEnum(SpindleState, uint8_t, unconfigured, stopped, forward, reverse);

class Spindle INHERIT_OBJECT_MODEL
{
private:
	void SetRpm(const uint32_t rpm) noexcept;

	PwmPort pwmPort, onOffPort, reverseNotForwardPort, spindleForwardPort, spindleReversePort;
	float currentRpm = 0, configuredRpm = 0, minRpm, maxRpm = 0;
	float currentPosition = 0, configuredPosition = 0;
	float currentTorque = 0, currentLoading = 0;
	float _offset = 0;
	PwmFrequency frequency;
	SpindleState state;

	int toolNumber;
	//char spindleLetters[2];				// The names of the spindle, with a null terminator
	//setting
	int DeltaIndex = 0;
	//0:odrive 1:delta
	int DriverType = 0;
	float GearRatio = 1;
	float PUU = 600;
	int Direction = -1;
	bool _Lock = false;
	// const char *Mcode_for_Odrive;
	// const char *Axis_for_Odrive;
	String<MachineNameLength> Mcode_for_Odrive_S;
	String<MachineNameLength> Axis_for_Odrive_S;

protected:
	DECLARE_OBJECT_MODEL

public:
	Spindle() noexcept;

	GCodeResult Configure(GCodeBuffer& gb, const StringRef& reply) THROWS(GCodeException);

	//uint32_t GetCurrentRpm() const noexcept { return currentRpm; }
	uint32_t GetMinRpm() const noexcept { return minRpm; }
	uint32_t GetMaxRpm() const noexcept { return maxRpm; }
	uint32_t GetRpm() const noexcept { return configuredRpm; }

	//Lock
	bool GetLock() const noexcept { return _Lock; }
	void SetLock(bool _value) noexcept { _Lock = _value; }
	//
	float GetCurrentRpm() const noexcept { return currentRpm; }
	void SetCurrentRpm(float _value) noexcept;
	//
	float GetconfiguredRpm() const noexcept { return configuredRpm; }
	void SetconfiguredRpm(float rpm) noexcept { configuredRpm = rpm; }
	//
	float GetCurrentPosition() const noexcept { return currentPosition; }
	void SetCurrentPosition(float _value) noexcept;
	//
	float GetconfiguredPosition() const noexcept { return configuredPosition; }
	void SetconfiguredPosition(float pos) noexcept { configuredPosition = pos; }
	//
	float GetcurrentTorque() const noexcept { return currentTorque; }
	void SetcurrentTorque(float _value) noexcept;
	//
	float GetcurrentLoading() const noexcept { return currentLoading; }
	void SetcurrentLoading(float Load) noexcept { currentLoading = Load; }
	//
	float GetOffset() const noexcept { return _offset; }
	void SetOffset(float _value) noexcept { _offset = _value; }
	//
	void SetDeltaIndex(int _value) noexcept { DeltaIndex = _value + 1; }
	void SetDriverType(int _value) noexcept { DriverType = _value; }
	int GetDriverType() noexcept { return DriverType; }
	void SetDirection(int _value) noexcept { Direction = _value; }
	void SetGearRatio(float _value) noexcept { GearRatio = _value; }
	void SetPUU(float _value) noexcept { PUU = _value; }
	//
	void SetOdriveParameter(const char* Mcode, const char* Axis) noexcept;
	void GetIdle(const StringRef& response_str) noexcept;
	void GetVelocity(const StringRef& response_str , int _value) noexcept;
	void GetTrap_traj_vel_limit(const StringRef& response_str , float _value) noexcept;
	void GetTrap_traj_accel_limit(const StringRef& response_str , float _value) noexcept;
	void GetTrap_traj_decel_limit(const StringRef& response_str , float _value) noexcept;
	void GetPosition(const StringRef& response_str , float _value) noexcept;
	// const char *GetspindleLetter() const noexcept { return spindleLetters; }
	// void SetspindleLetter(const char* sdil) noexcept;
	//
	void TurnOff() noexcept;
	bool GetIsReachRPM() noexcept;
	void GetReachRPMstring(const StringRef& response_str) noexcept;


	bool IsValidRpm(const uint32_t rpm) const noexcept { return rpm >= minRpm && rpm <= maxRpm; }
	void SetConfiguredRpm(const uint32_t rpm, const bool updateCurrentRpm) noexcept;
	SpindleState GetState() const noexcept { return state; }
	void SetState(const SpindleState newState) noexcept;
};

#endif
