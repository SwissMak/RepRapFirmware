// CNC machine function for GCode class

#include "GCodes.h"

#include "GCodeBuffer/GCodeBuffer.h"
#include "GCodeException.h"
#include "GCodeQueue.h"
#include "Heating/Heat.h"
//#if HAS_LINUX_INTERFACE
//# include "Linux/LinuxInterface.h"
//#endif
#include "Movement/Move.h"
#include "Network.h"
#include "Platform/Scanner.h"
#include "PrintMonitor/PrintMonitor.h"
#include "Platform/RepRap.h"
#include "Tools/Tool.h"
#include "Endstops/ZProbe.h"
#include "FilamentMonitors/FilamentMonitor.h"
#include "General/IP4String.h"
#include "Movement/StepperDrivers/DriverMode.h"
#include "Version.h"
#include <string>
#include "CAN/CanInterface.h"

#if SUPPORT_IOBITS
# include "Platform/PortControl.h"
#endif

#if HAS_WIFI_NETWORKING
# include "FirmwareUpdater.h"
#endif

#if SUPPORT_12864_LCD
# include "Display/Display.h"
#endif

#if SUPPORT_DOTSTAR_LED
# include "Fans/DotStarLed.h"
#endif

#if SUPPORT_CAN_EXPANSION
# include <CAN/CanInterface.h>
# include <CAN/ExpansionManager.h>
#endif

#include <utility>			// for std::swap

#define M_PI 3.14159265358979323846

//close all
void GCodes::CloseGodeMultiple() noexcept
{
	g0_active = false;
	g1_active = false;
	g2_active = false;
	g3_active = false;
	g32_active = false;
    g76_active = false;
    g83_active = false;
    g84_active = false;
}

void GCodes::CloseModalCommand() noexcept
{
	g0_active = false;
	g1_active = false;
	g2_active = false;
	g3_active = false;
	g32_active = false;
    g76_active = false;
}

//Print History
void GCodes::PrintGcodeHistory(GCodeBuffer& gb) THROWS(GCodeException)
{
	//GetCommandLetter
	//GetCommandNumber
	int code = gb.GetCommandNumber();
	float Mcode = gb.GetModalCommandNumber();

	//debugPrintf("code = %d \n", code);
	//debugPrintf("Mcode = %f \n", Mcode);

	if(code != 1003){
		//GetLineNumber
		char str_temp[255] = "N";
		SafeSnprintf(str_temp, sizeof(str_temp), "%s%d ", str_temp, gb.CurrentFileMachineState().lineNumber);
		//debugPrintf("Str = %s \n", str_temp);

		switch(gb.GetCommandLetter()){
				//Normal Command
				case 'G':
					SafeSnprintf(str_temp, sizeof(str_temp), "%s G%d ", str_temp, code);
					break;

				case 'M':
					SafeSnprintf(str_temp, sizeof(str_temp), "%s M%d ", str_temp, code);
					break;

				case 'T':
					SafeSnprintf(str_temp, sizeof(str_temp), "%s T%d ", str_temp, code);
					break;

				case 'S':
					SafeSnprintf(str_temp, sizeof(str_temp), "%s S%d ", str_temp, code);
					break;

				//Modal Command
				case 'X':
					SafeSnprintf(str_temp, sizeof(str_temp), "%s X%.3f ", str_temp, (double)Mcode);
					break;

				case 'Y':
					SafeSnprintf(str_temp, sizeof(str_temp), "%s Y%.3f ", str_temp, (double)Mcode);
					break;

				case 'Z':
					SafeSnprintf(str_temp, sizeof(str_temp), "%s Z%.3f ", str_temp, (double)Mcode);
					break;

				case 'U':
					SafeSnprintf(str_temp, sizeof(str_temp), "%s U%.3f ", str_temp, (double)Mcode);
					break;

				case 'V':
					SafeSnprintf(str_temp, sizeof(str_temp), "%s V%.3f ", str_temp, (double)Mcode);
					break;

				case 'W':
					SafeSnprintf(str_temp, sizeof(str_temp), "%s W%.3f ", str_temp, (double)Mcode);
					break;

				case 'A':
					SafeSnprintf(str_temp, sizeof(str_temp), "%s A%.3f ", str_temp, (double)Mcode);
					break;

				case 'B':
					SafeSnprintf(str_temp, sizeof(str_temp), "%s B%.3f ", str_temp, (double)Mcode);
					break;

				case 'C':
					SafeSnprintf(str_temp, sizeof(str_temp), "%s C%.3f ", str_temp, (double)Mcode);
					break;

				case 'D':
					SafeSnprintf(str_temp, sizeof(str_temp), "%s D%.3f ", str_temp, (double)Mcode);
					break;
			}

			//float Parameter & Not CommandLetter
			//float FloatParameter;
			if (gb.Seen('X') && gb.GetCommandLetter() != 'X'){
				SafeSnprintf(str_temp, sizeof(str_temp), "%s X%.3f ", str_temp, gb.GetDistance());
			}
			if (gb.Seen('Y') && gb.GetCommandLetter() != 'Y'){
				SafeSnprintf(str_temp, sizeof(str_temp), "%s Y%.3f ", str_temp, gb.GetDistance());
			}
			if (gb.Seen('Z') && gb.GetCommandLetter() != 'Z'){
				SafeSnprintf(str_temp, sizeof(str_temp), "%s Z%.3f ", str_temp, gb.GetDistance());
			}
			if (gb.Seen('U') && gb.GetCommandLetter() != 'U'){
				SafeSnprintf(str_temp, sizeof(str_temp), "%s U%.3f ", str_temp, gb.GetDistance());
			}
			if (gb.Seen('V') && gb.GetCommandLetter() != 'V'){
				SafeSnprintf(str_temp, sizeof(str_temp), "%s V%.3f ", str_temp, gb.GetDistance());
			}
			if (gb.Seen('W') && gb.GetCommandLetter() != 'W'){
				SafeSnprintf(str_temp, sizeof(str_temp), "%s W%.3f ", str_temp, gb.GetDistance());
			}
			if (gb.Seen('A') && gb.GetCommandLetter() != 'A'){
				SafeSnprintf(str_temp, sizeof(str_temp), "%s A%.3f ", str_temp, gb.GetDistance());
			}
			if (gb.Seen('B') && gb.GetCommandLetter() != 'B'){
				SafeSnprintf(str_temp, sizeof(str_temp), "%s B%.3f ", str_temp, gb.GetDistance());
			}
			if (gb.Seen('C') && gb.GetCommandLetter() != 'C'){
				SafeSnprintf(str_temp, sizeof(str_temp), "%s C%.3f ", str_temp, gb.GetDistance());
			}
			if (gb.Seen('D') && gb.GetCommandLetter() != 'D'){
				SafeSnprintf(str_temp, sizeof(str_temp), "%s D%.3f ", str_temp, gb.GetDistance());
			}

			//float Parameter
			if (gb.Seen('F')){
				SafeSnprintf(str_temp, sizeof(str_temp), "%s F%.3f ", str_temp, gb.GetFValue());
			}
//			if (gb.Seen('H')){
//				SafeSnprintf(str_temp, sizeof(str_temp), "%s H%.3f ", str_temp, gb.GetFValue());
//			}
			if (gb.Seen('I')){
				SafeSnprintf(str_temp, sizeof(str_temp), "%s I%.3f ", str_temp, gb.GetFValue());
			}
			if (gb.Seen('J')){
				SafeSnprintf(str_temp, sizeof(str_temp), "%s J%.3f ", str_temp, gb.GetFValue());
			}
			if (gb.Seen('K')){
				SafeSnprintf(str_temp, sizeof(str_temp), "%s K%.3f ", str_temp, gb.GetFValue());
			}
//			if (gb.Seen('O')){
//				SafeSnprintf(str_temp, sizeof(str_temp), "%s O%.3f ", str_temp, gb.GetFValue());
//			}
//			if (gb.Seen('P')){
//				SafeSnprintf(str_temp, sizeof(str_temp), "%s P%.3f ", str_temp, gb.GetFValue());
//			}
//			if (gb.Seen('Q')){
//				SafeSnprintf(str_temp, sizeof(str_temp), "%s Q%.3f ", str_temp, gb.GetFValue());
//			}
			if (gb.Seen('R')){
				SafeSnprintf(str_temp, sizeof(str_temp), "%s R%.3f ", str_temp, gb.GetFValue());
			}
//
//			//int Parameter
//			if (gb.Seen('L')){
//				SafeSnprintf(str_temp, sizeof(str_temp), "%s L%d", str_temp, gb.GetIValue());
//			}
			if (gb.Seen('S') && gb.GetCommandLetter() != 'S'){
				SafeSnprintf(str_temp, sizeof(str_temp), "%s S%d ", str_temp, gb.GetIValue());
			}
			SafeSnprintf(str_temp, sizeof(str_temp), "%s\n", str_temp);

			MessageType type = ErrorMessage;
			String<StringLength256> activeComm;
			activeComm.copy(str_temp);
			platform.Message(type, activeComm.c_str());
	}

	/////Print
	//	MessageType type = GenericMessage;
	//	String<StringLength256> activeComm;
	//	activeComm.copy("Print History\n");
	//	platform.Message(type, activeComm.c_str());
	/////Print
}


//StringParser 'T'
void GCodes::ParserT(GCodeBuffer& gb, const StringRef& reply) THROWS(GCodeException)
{
	char str_temp[255] = "T";
	int code = gb.GetCommandNumber();
	if(code>=100){
		code /= 100;
	}
	SafeSnprintf(str_temp, sizeof(str_temp), "%s%d\n", str_temp, code);
	//debugPrintf("Str = %s \n", str_temp);
	gb.PutAndDecode(str_temp);
	reply.printf("");
	HandleReply(gb, GCodeResult::ok, reply.c_str());
}

//modal command
void GCodes::ModalCmmSpindle(GCodeBuffer& gb, const StringRef& reply) THROWS(GCodeException)
{
	char str_temp[255] = "M3 S";
	int code = gb.GetCommandNumber();
	SafeSnprintf(str_temp, sizeof(str_temp), "%s%d\n", str_temp, code);
	gb.PutAndDecode(str_temp);
	//debugPrintf("Str = %s \n", str_temp);
	reply.printf("");
	HandleReply(gb, GCodeResult::ok, reply.c_str());
}

void GCodes::ModalCmmMove(GCodeBuffer& gb, const StringRef& reply) THROWS(GCodeException)
{
	//DecodeCommand
	char str_temp[255] = "";

	if(g0_active == true)	SafeSnprintf(str_temp, sizeof(str_temp), "G0 ");
	else if(g1_active == true)	SafeSnprintf(str_temp, sizeof(str_temp), "G1 ");
	else if(g2_active == true)	SafeSnprintf(str_temp, sizeof(str_temp), "G2 ");
	else if(g3_active == true)	SafeSnprintf(str_temp, sizeof(str_temp), "G3 ");

	float code = gb.GetModalCommandNumber();
	//debugPrintf("code = %.3f \n", code);

	float ModalX, ModalY, ModalZ, ModalU, ModalV, ModalW;
	double ModalA, ModalB, ModalC, ModalD;
	int ModalF;

	switch(gb.GetCommandLetter()){
		case 'X':
			SafeSnprintf(str_temp, sizeof(str_temp), "%s X%.3f ", str_temp, (double)code);
			break;

		case 'Y':
			SafeSnprintf(str_temp, sizeof(str_temp), "%s Y%.3f ", str_temp, (double)code);
			break;

		case 'Z':
			SafeSnprintf(str_temp, sizeof(str_temp), "%s Z%.3f ", str_temp, (double)code);
			break;

		case 'U':
			SafeSnprintf(str_temp, sizeof(str_temp), "%s U%.3f ", str_temp, (double)code);
			break;

		case 'V':
			SafeSnprintf(str_temp, sizeof(str_temp), "%s V%.3f ", str_temp, (double)code);
			break;

		case 'W':
			SafeSnprintf(str_temp, sizeof(str_temp), "%s W%.3f ", str_temp, (double)code);
			break;

		case 'A':
			SafeSnprintf(str_temp, sizeof(str_temp), "%s A%.3f ", str_temp, (double)code);
			break;

		case 'B':
			SafeSnprintf(str_temp, sizeof(str_temp), "%s B%.3f ", str_temp, (double)code);
			break;

		case 'C':
			SafeSnprintf(str_temp, sizeof(str_temp), "%s C%.3f ", str_temp, (double)code);
			break;

		case 'D':
			SafeSnprintf(str_temp, sizeof(str_temp), "%s D%.3f ", str_temp, (double)code);
			break;
	}
	//debugPrintf("Str = %s \n", str_temp);
	if (gb.GetCommandLetter() != 'X' && gb.Seen('X')){
		ModalX = gb.GetDistance();
		debugPrintf("X = %.3f \n", gb.GetCommandLetter());
		SafeSnprintf(str_temp, sizeof(str_temp), "%s X%.3f ", str_temp, (double)ModalX);
	}
	if (gb.GetCommandLetter() != 'Y' && gb.Seen('Y')){
		ModalY = gb.GetDistance();
		SafeSnprintf(str_temp, sizeof(str_temp), "%s Y%.3f ", str_temp, (double)ModalY);
	}
	if (gb.GetCommandLetter() != 'Z' && gb.Seen('Z')){
		ModalZ = gb.GetDistance();
		SafeSnprintf(str_temp, sizeof(str_temp), "%s Z%.3f ", str_temp, (double)ModalZ);
	}
	if (gb.GetCommandLetter() != 'U' && gb.Seen('U')){
		ModalU = gb.GetDistance();
		SafeSnprintf(str_temp, sizeof(str_temp), "%s U%.3f ", str_temp, (double)ModalU);
	}
	if (gb.GetCommandLetter() != 'V' && gb.Seen('V')){
		ModalV = gb.GetDistance();
		SafeSnprintf(str_temp, sizeof(str_temp), "%s V%.3f ", str_temp, (double)ModalV);
	}
	if (gb.GetCommandLetter() != 'W' && gb.Seen('W')){
		ModalW = gb.GetDistance();
		SafeSnprintf(str_temp, sizeof(str_temp), "%s W%.3f ", str_temp, (double)ModalW);
	}
	if (gb.GetCommandLetter() != 'A' && gb.Seen('A')){
		ModalA = gb.GetDistance();
		SafeSnprintf(str_temp, sizeof(str_temp), "%s A%.3f ", str_temp, (double)ModalA);
	}
	if (gb.GetCommandLetter() != 'B' && gb.Seen('B')){
		ModalB = gb.GetDistance();
		SafeSnprintf(str_temp, sizeof(str_temp), "%s B%.3f ", str_temp, (double)ModalB);
	}
	if (gb.GetCommandLetter() != 'C' && gb.Seen('C')){
		ModalC = gb.GetDistance();
		SafeSnprintf(str_temp, sizeof(str_temp), "%s C%.3f ", str_temp, (double)ModalC);
	}
	if (gb.GetCommandLetter() != 'D' && gb.Seen('D')){
		ModalD = gb.GetDistance();
		SafeSnprintf(str_temp, sizeof(str_temp), "%s D%.3f ", str_temp, (double)ModalD);
	}
	if (gb.Seen('I')){
		ModalD = gb.GetFValue();
		SafeSnprintf(str_temp, sizeof(str_temp), "%s I%.3f ", str_temp, (double)ModalD);
	}
	if (gb.Seen('J')){
		ModalD = gb.GetFValue();
		SafeSnprintf(str_temp, sizeof(str_temp), "%s J%.3f ", str_temp, (double)ModalD);
	}
	if (gb.Seen('K')){
		ModalD = gb.GetFValue();
		SafeSnprintf(str_temp, sizeof(str_temp), "%s K%.3f ", str_temp, (double)ModalD);
	}
	if (gb.Seen('R')){
		ModalD = gb.GetFValue();
		SafeSnprintf(str_temp, sizeof(str_temp), "%s R%.3f ", str_temp, (double)ModalD);
	}
	if (gb.Seen('F')){
		ModalF = gb.GetIValue();
		SafeSnprintf(str_temp, sizeof(str_temp), "%s F%d ", str_temp, ModalF);
	}
	SafeSnprintf(str_temp, sizeof(str_temp), "%s\n", str_temp);
	gb.PutAndDecode(str_temp);

	//debugPrintf("Str = %s \n", str_temp);

	reply.printf("");
	HandleReply(gb, GCodeResult::ok, reply.c_str());
}


//G32
GCodeResult GCodes::SetG32(GCodeBuffer& gb, const StringRef& reply) THROWS(GCodeException)
{   
    _Pitch = 0;
    _total_Turns = 0;
    CloseGodeMultiple();
    g32_active = true;
	return GCodeResult::ok;
}

//position ctl mode
bool GCodes::RunG32(GCodeBuffer& gb, const StringRef& reply) THROWS(GCodeException)
{
    const int code = gb.GetCommandNumber();
    //write the entire file
    char _filename[20] = "/macros/G32.g";
    char str_temp[255];
    _real_Z = 0;
    FileStore * const f = platform.OpenSysFile(_filename, OpenMode::write);
    if (f == nullptr)
    {
        reply.printf("G32 file not closed");
        HandleReply(gb, GCodeResult::error, reply.c_str());
        return true;
    }
    else
    {   
        if (gb.Seen('F'))
        {
            _Pitch = gb.GetFValue();
        }
        //Spindle RPM
        if (gb.Seen('S'))
        {
            _spindleRPM = gb.GetIValue();
        }else{
            reply.copy("S parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        //check datas
        if(_Pitch == 0){
            reply.printf("pitch cannot be 0");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        bool _flag = false;
        //get Now X position
        float _real_X=0;
        for (size_t axis = 0; axis < numVisibleAxes; ++axis)
        {
            if(axisLetters[axis] == 'x' || axisLetters[axis] == 'X'){
                _real_X = HideNan(GetUserCoordinate(axis));
                _flag = true;
                break;
            }
        }
        if(_flag == false){
            reply.printf("G32-You need set X axis!");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        _flag = false;
        //get Now X position
        float _real_Z=0;
        for (size_t axis = 0; axis < numVisibleAxes; ++axis)
        {
            if(axisLetters[axis] == 'z' || axisLetters[axis] == 'Z'){
                _real_Z = HideNan(GetUserCoordinate(axis));
                _flag = true;
                break;
            }
        }
        if(_flag == false){
            reply.printf("G32-You need set Z axis!");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        //
        float _Final_X = _real_X;
        float _Final_Z = _real_Z;
        float _turns = 0;
        _FeedRate = (int)_spindleRPM * _Pitch;
        //start
        f->Write("G1");
        if (gb.Seen('X'))
        {
            _X_abs = gb.GetDistance();
            _Final_X = _X_abs;
            SafeSnprintf(str_temp, sizeof(str_temp), " X%.3f F%d", (double)_Final_X, _FeedRate);
            f->Write(str_temp);
        }else if (gb.Seen('U'))
        {
            _U_inc = gb.GetDistance();
            _Final_X = _real_X + _U_inc;
            SafeSnprintf(str_temp, sizeof(str_temp), " X%.3f F%d", (double)_Final_X, _FeedRate);
            f->Write(str_temp);
        }
        
        if (gb.Seen('Z'))
        {
            _Endpoint_of_thread = fabsf(_Z_abs-_real_Z);
            _turns = _Endpoint_of_thread/_Pitch;
            _total_Turns += _turns;
            _Z_abs = gb.GetDistance();
            _Final_Z = _Z_abs;
            SafeSnprintf(str_temp, sizeof(str_temp), " Z%.3f F%d -C%.3f", (double)_Final_Z, _FeedRate, (double)_total_Turns);
            f->Write(str_temp);
        }else if (gb.Seen('W'))
        {
            _Endpoint_of_thread = fabsf(_W_inc);
            _turns = _Endpoint_of_thread/_Pitch;
            _total_Turns += _turns;
            _W_inc = gb.GetDistance();
            _Final_Z = _real_Z + _W_inc;
            SafeSnprintf(str_temp, sizeof(str_temp), " Z%.3f F%d -C%.3f", (double)_Final_Z, _FeedRate, (double)_total_Turns);
            f->Write(str_temp);
        }
        f->Write("\n");
        
        if (!f->Close())
        {
            reply.printf("G32 file not closed");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        DoFileMacro(gb, _filename, true, code);
        return true;
    }
}
///////////////////////////////////////////////////////////////////
//G76
GCodeResult GCodes::SetG76(GCodeBuffer& gb, const StringRef& reply) THROWS(GCodeException)
{

	if (gb.Seen('X'))
    {
        //Start = 11 ; Target = 10
    	//if diameter : start=11 end=10 back=11
    	//if radius : start=22 end=10 back=22
    	_dia_of_thread = gb.GetFValue();
    	//_dia_of_thread = gb.GetDistance();
    	//debugPrintf("dia = %f \n", _dia_of_thread);
    }else{
        reply.copy("X parameter not found");
        return GCodeResult::error;
    }
    if (gb.Seen('Z'))
    {
        _SZ_76 = gb.GetFValue();
        //_SZ_76 = gb.GetDistance();
        //debugPrintf("_SZ_76 = %f \n", _SZ_76);
    }else{
        reply.copy("Z parameter not found");
        return GCodeResult::error;
    }
    if (gb.Seen('Q'))
    {
    	_EZ_76 = gb.GetFValue();
    	//_EZ_76 = gb.GetDistance();
    	//gb.GetDistance()
    	//debugPrintf("_EZ_76 = %f \n", _EZ_76);
    }else{
        reply.copy("Q parameter not found");
        return GCodeResult::error;
    }
    if (gb.Seen('P'))
    {
        _Depth_of_thread = gb.GetFValue();
    	//_Depth_of_thread = gb.GetDistance();
        //debugPrintf("Depth = %f \n", _Depth_of_thread);
    }else{
        reply.copy("P parameter not found");
        return GCodeResult::error;
    }
    /////unit is mm
    if(gb.LatestMachineState().usingInches == false){
    	if (gb.Seen('F'))
    	{
    	    _Pitch = gb.GetFValue();
    	    //_Pitch = gb.GetDistance();
    	}else{
    	    reply.copy("F parameter not found");
    	    return GCodeResult::error;
    	}
    }
    /////unit is inch
    else{
    	if (gb.Seen('I'))
    	{
    	    _Number_of_thread = gb.GetFValue();
    	    //debugPrintf("Nun = %f \n", _Number_of_thread);
    	}else{
    	    reply.copy("I parameter not found");
    	    return GCodeResult::error;
    	}
    }
    //Spindle RPM
    if (gb.Seen('S'))
    {
        _spindleRPM = gb.GetIValue();
    }else{
        reply.copy("S parameter not found");
        return GCodeResult::error;
    }
    CloseGodeMultiple();
    g76_active = true;
	return GCodeResult::ok;
}

//position ctl mode
bool GCodes::RunG76(GCodeBuffer& gb, const StringRef& reply) THROWS(GCodeException)
{
    const int code = gb.GetCommandNumber();
    //write the entire file
    char _filename[20] = "/macros/G76.g";
    String<StringLength256> activeComm;
    FileStore * const f = platform.OpenSysFile(_filename, OpenMode::write);
    if (f == nullptr)
    {
        reply.printf("G76 file not closed");
        HandleReply(gb, GCodeResult::error, reply.c_str());
        return true;
    }
    else
    {   
        //check datas
        if(_Pitch == 0 && gb.LatestMachineState().usingInches == false){
            reply.printf("pitch of thread cannot be 0");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        if(_Number_of_thread == 0 && gb.LatestMachineState().usingInches == true){
            reply.printf("number of thread cannot be 0");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        bool _flag = false;
        //get Now X position
        float _real_X=0;
        for (size_t axis = 0; axis < numVisibleAxes; ++axis)
        {
            if(axisLetters[axis] == 'x' || axisLetters[axis] == 'X'){
            	//_real_X = HideNan(GetUserCoordinate(axis));
            	if(IsDiameterMode == true) _real_X = HideNan(GetUserCoordinate(axis))*2;
            	else _real_X = HideNan(GetUserCoordinate(axis));
                _flag = true;
                break;
            }
        }
        if(_flag == false){
            reply.printf("G76-You need set X axis!");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }

        /////unit is inch
        if(gb.LatestMachineState().usingInches == true) _real_X /= 25.4;
        //debugPrintf("real = %f \n", _real_X);
        int iDirection = 1;
        float _StartX = _real_X;
        float _sumX = 0;
        //_dia_of_thread = _dia_of_thread/2;
        if(_StartX > _dia_of_thread){iDirection = -1;}else{iDirection = 1;}
        _Depth_of_hole = fabsf(_dia_of_thread-_StartX);
        _Endpoint_of_thread = fabsf(_EZ_76-_SZ_76);
        //float __feedrate = _spindleRPM * _Pitch;
        float __feedrate =0;
        /////unit is mm
        if(gb.LatestMachineState().usingInches == false){
        	__feedrate = _spindleRPM * 360;
        }
        /////unit is inch
        else{
        	__feedrate = _spindleRPM * 360/25.4;
        }

        float _turns=0.0 , nowTurns=0.0;
        activeComm.copy("G28 A\n");
        activeComm.cat("G4 P3000\n");
        activeComm.cat("M1102\n");
        f->Write(activeComm.c_str());

        while(_Depth_of_hole > _sumX){
            activeComm.copy("");
            _sumX += _Depth_of_thread;
            if(_Depth_of_hole >= _sumX){                
                _real_X = _StartX + _sumX * iDirection;
            }
            else{
                _real_X = _StartX + _Depth_of_hole * iDirection;
            }
            nowTurns = ceil(nowTurns);            
            //to Z start
            activeComm.catf("G0 Z%.3f C%.3f\n", (double)_SZ_76, (double)nowTurns * 360);
            //to X start
            activeComm.catf("G0 X%.3f\n", (double)_real_X);
            activeComm.cat("G4 P10\n");

            //rotation
            /////unit is mm
            if(gb.LatestMachineState().usingInches == false) _turns = _Endpoint_of_thread/_Pitch;
            /////unit is inch
            else _turns = _Endpoint_of_thread * _Number_of_thread;

            //debugPrintf("Endpoint = %f \n", _Endpoint_of_thread);
            //debugPrintf("turn = %f \n", _turns);
            nowTurns += _turns;
            //debugPrintf("nowturn = %f \n", nowTurns);
            activeComm.catf("G1 Z%.3f C%.3f F%.3f\n",  (double)_EZ_76 , (double)nowTurns * 360 , (double)__feedrate);
            activeComm.cat("G4 P10\n");
            //back to X start
            activeComm.catf("G0 X%.3f\n", (double)_StartX);
            activeComm.cat("G4 P300\n");
            //
            f->Write(activeComm.c_str());
        }
        activeComm.copy("");
        nowTurns = ceil(nowTurns);
        //to Z start
        activeComm.catf("G0 Z%.3f C%.3f\n", (double)_SZ_76, (double)nowTurns * 360);
        activeComm.cat("G4 P300\n");
        activeComm.cat("G92 C0\n");
        f->Write(activeComm.c_str());
        //to Z start
        activeComm.catf("G0 Z%.3f X%.3f\n",  (double)_SZ_76 , (double)_StartX );
        activeComm.copy("M5002\n");
        //activeComm.cat("M1100\n");
        f->Write(activeComm.c_str());
        //
        if (!f->Close())
        {
            reply.printf("G76 file not closed");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        DoFileMacro(gb, _filename, true, code);
        g76_active = false;
        return true;
    }
}

// //position ctl mode
// bool GCodes::RunG76(GCodeBuffer& gb, const StringRef& reply) THROWS(GCodeException)
// {
//     const int code = gb.GetCommandNumber();
//     //write the entire file
//     char _filename[20] = "/macros/G76.g";
//     char str_temp[255];
//     _sumQ = 0;
//     _real_Z = 0;
//     FileStore * const f = platform.OpenSysFile(_filename, OpenMode::write);
//     if (f == nullptr)
//     {
//         reply.printf("G76 file not closed");
//         HandleReply(gb, GCodeResult::error, reply.c_str());
//         return true;
//     }
//     else
//     {   
//         //check datas
//         if(_Pitch == 0){
//             reply.printf("pitch of thread cannot be 0");
//             HandleReply(gb, GCodeResult::error, reply.c_str());
//             return true;
//         }
//         bool _flag = false;
//         //get Now X position
//         float _real_X=0;
//         for (size_t axis = 0; axis < numVisibleAxes; ++axis)
//         {
//             if(axisLetters[axis] == 'x' || axisLetters[axis] == 'X'){
//                 _real_X = HideNan(GetUserCoordinate(axis));
//                 _flag = true;
//                 break;
//             }
//         }
//         if(_flag == false){
//             reply.printf("G76-You need set X axis!");
//             HandleReply(gb, GCodeResult::error, reply.c_str());
//             return true;
//         }
//         //
//         int iDirection = 1;
//         float _StartX = _real_X;
//         float _sumX = 0;
//         _dia_of_thread = _dia_of_thread/2;
//         if(_StartX > _dia_of_thread){
//             iDirection = -1;
//         }else{
//             iDirection = 1;
//         }
//         _Depth_of_hole = fabsf(_dia_of_thread-_StartX);
//         _Endpoint_of_thread = fabsf(_EZ_76-_SZ_76);
//         float __feedrate = _spindleRPM * _Pitch;
        
//         while(_Depth_of_hole > _sumX){
//             _sumX += _Depth_of_thread;
//             if(_Depth_of_hole >= _sumX){                
//                 _real_X = _StartX + _sumX * iDirection;
//             }
//             else{
//                 _real_X = _StartX + _Depth_of_hole * iDirection;
//             }
//             //to Z start
//             SafeSnprintf(str_temp, sizeof(str_temp), "G0 Z%.3f\n", (double)_SZ_76);
//             f->Write(str_temp);
//             //
//             f->Write("G4 P10\n");
//             f->Write("M19 S0\n");
//             f->Write("G4 P3000\n");
//             //to X start
//             SafeSnprintf(str_temp, sizeof(str_temp), "G0 X%.3f\n", (double)_real_X);
//             f->Write(str_temp);
//             f->Write("G4 P10\n");
//             //
//             SafeSnprintf(str_temp, sizeof(str_temp), "M3 S%.3f\n", (double)_spindleRPM);
//             f->Write(str_temp);
//             //rotation
//             SafeSnprintf(str_temp, sizeof(str_temp), "G1 Z%.3f F%.3f\n", (double)_EZ_76,(double)__feedrate);
//             f->Write(str_temp);
//             f->Write("G4 P10\n");
//             //back to X start
//             SafeSnprintf(str_temp, sizeof(str_temp), "G0 X%.3f\n", (double)_StartX);
//             f->Write(str_temp);
//             //
//             f->Write("M5\n");
//         }
//         //to Z start
//             SafeSnprintf(str_temp, sizeof(str_temp), "G0 Z%.3f X%.3f\n", (double)_StartZ, (double)_StartX);
//             f->Write(str_temp);
//         //
//         if (!f->Close())
//         {
//             reply.printf("G76 file not closed");
//             HandleReply(gb, GCodeResult::error, reply.c_str());
//             return true;
//         }
//         DoFileMacro(gb, _filename, true, code);
//         g76_active = false;
//         return true;
//     }
// }

// //position ctl mode 2021/3/10
// bool GCodes::RunG76(GCodeBuffer& gb, const StringRef& reply) THROWS(GCodeException)
// {
//     const int code = gb.GetCommandNumber();
//     //write the entire file
//     char _filename[20] = "/macros/G76.g";
//     char str_temp[255];
//     _sumQ = 0;
//     _real_Z = 0;
//     FileStore * const f = platform.OpenSysFile(_filename, OpenMode::write);
//     if (f == nullptr)
//     {
//         reply.printf("G76 file not closed");
//         HandleReply(gb, GCodeResult::error, reply.c_str());
//         return true;
//     }
//     else
//     {   
//         //check datas
//         if(_Pitch == 0){
//             reply.printf("pitch of thread cannot be 0");
//             HandleReply(gb, GCodeResult::error, reply.c_str());
//             return true;
//         }
//         bool _flag = false;
//         //get Now X position
//         float _real_X=0;
//         for (size_t axis = 0; axis < numVisibleAxes; ++axis)
//         {
//             if(axisLetters[axis] == 'x' || axisLetters[axis] == 'X'){
//                 _real_X = HideNan(GetUserCoordinate(axis));
//                 _flag = true;
//                 break;
//             }
//         }
//         if(_flag == false){
//             reply.printf("G76-You need set X axis!");
//             HandleReply(gb, GCodeResult::error, reply.c_str());
//             return true;
//         }
//         //
//         int iDirection = 1;
//         float _StartX = _real_X;
//         float _StartZ = _SZ_76;
//         float _sumX = 0;
//         if(_StartX > _dia_of_thread){
//             _Depth_of_hole = _StartX - _dia_of_thread;
//             iDirection = -1;
//         }else{
//             _Depth_of_hole = _dia_of_thread - _StartX;
//             iDirection = 1;
//         }
//         _Endpoint_of_thread = fabsf(_EZ_76-_SZ_76);
//         float _turns = _Endpoint_of_thread/_Pitch;
//         float _turnsSum = _turns;
//         float __feedrate = _spindleRPM * _Pitch;
//         f->Write("M19 A0\n");
//         f->Write("G4 P200\n");
//         f->Write("G92 C0\n");
//         f->Write("G4 P10\n");
//         while(_Depth_of_hole > _sumX){
//             _sumX += _Depth_of_thread;
//             if(_Depth_of_hole >= _sumX){                
//                 _real_X = _StartX + _sumX * iDirection;
//             }
//             else{
//                 _real_X = _StartX + _Depth_of_hole * iDirection;
//             }
//             //to Z start
//             SafeSnprintf(str_temp, sizeof(str_temp), "G0 Z%.3f\n", (double)_StartZ);
//             f->Write(str_temp);
//             //to X start
//             SafeSnprintf(str_temp, sizeof(str_temp), "G0 X%.3f\n", (double)_real_X);
//             f->Write(str_temp);
//             f->Write("G4 P10\n");
//             //rotation
//             SafeSnprintf(str_temp, sizeof(str_temp), "G1 Z%.3f C-%.3f F%.3f\n", (double)(_StartZ-_Endpoint_of_thread), (double)_turnsSum ,(double)__feedrate);
//             f->Write(str_temp);
//             f->Write("G4 P10\n");
//             //back to X start
//             SafeSnprintf(str_temp, sizeof(str_temp), "G0 X%.3f Z%.3f\n", (double)_StartX, (double)(_StartZ-_Endpoint_of_thread-5));
//             f->Write(str_temp);
//             //
//             _turnsSum += _turns;
//         }
//         //
//         if (!f->Close())
//         {
//             reply.printf("G76 file not closed");
//             HandleReply(gb, GCodeResult::error, reply.c_str());
//             return true;
//         }
//         DoFileMacro(gb, _filename, true, code);
//         g76_active = false;
//         return true;
//     }
// }

//keep spin mode
// bool GCodes::RunG76(GCodeBuffer& gb, const StringRef& reply) THROWS(GCodeException)
// {
//     const int code = gb.GetCommandNumber();
//     //write the entire file
//     char _filename[20] = "/macros/G76.g";
//     char str_temp[255];
//     _sumQ = 0;
//     _real_Z = 0;
//     FileStore * const f = platform.OpenSysFile(_filename, OpenMode::write);
//     if (f == nullptr)
//     {
//         reply.printf("G76 file not closed");
//         HandleReply(gb, GCodeResult::error, reply.c_str());
//         return true;
//     }
//     else
//     {   
//         //check datas
//         if(_Pitch == 0){
//             reply.printf("pitch of thread cannot be 0");
//             HandleReply(gb, GCodeResult::error, reply.c_str());
//             return true;
//         }
//         bool _flag = false;
//         //get Now X position
//         float _real_X=0;
//         for (size_t axis = 0; axis < numVisibleAxes; ++axis)
//         {
//             if(axisLetters[axis] == 'x' || axisLetters[axis] == 'X'){
//                 _real_X = HideNan(GetUserCoordinate(axis));
//                 _flag = true;
//                 break;
//             }
//         }
//         if(_flag == false){
//             reply.printf("G76-You need set X axis!");
//             HandleReply(gb, GCodeResult::error, reply.c_str());
//             return true;
//         }
//         _flag = false;
//         //get Now Z position        
//         for (size_t axis = 0; axis < numVisibleAxes; ++axis)
//         {
//             if(axisLetters[axis] == 'z' || axisLetters[axis] == 'Z'){
//                 _real_Z = HideNan(GetUserCoordinate(axis));
//                 _flag = true;
//                 break;
//             }
//         }
//         if(_flag == false){
//             reply.printf("G76-You need set Z axis!");
//             HandleReply(gb, GCodeResult::error, reply.c_str());
//             return true;
//         }
//         //
//         int iDirection = 1;
//         float _StartX = _real_X;
//         float _StartZ = _real_Z;
//         float _sumX = 0;
//         if(_StartX > _dia_of_thread){
//             _Depth_of_hole = _StartX - _dia_of_thread;
//             iDirection = -1;
//         }else{
//             _Depth_of_hole = _dia_of_thread - _StartX;
//             iDirection = 1;
//         }
//         _FeedRate = _Pitch * _spindleRPM;
//         //SETTING MILLING SPINDLE TO POS 0
//         SafeSnprintf(str_temp, sizeof(str_temp), "M3 S%d\n", _spindleRPM);
//         f->Write(str_temp);
//         while(_Depth_of_hole > _sumX){							
//             //move Z to R - Q
//             if(_sumX == 0){
//                 _sumX += _Depth_of_first_cut;
//             }else{
//                 _sumX += _Depth_of_thread;
//             }            
//             if(_Depth_of_hole >= _sumX){                
//                 _real_X = _StartX + _sumX * iDirection;
//             }
//             else{
//                 _real_X = _StartX + _Depth_of_hole * iDirection;
//             }
//             //to Z start
//             SafeSnprintf(str_temp, sizeof(str_temp), "G1 Z%.3f F%d\n", (double)_StartZ, _FeedRate*2);
//             f->Write(str_temp);
//             //to X start
//             SafeSnprintf(str_temp, sizeof(str_temp), "G1 X%.3f F%d\n", (double)_real_X, _FeedRate*2);
//             f->Write(str_temp);
//             //rotation
//             SafeSnprintf(str_temp, sizeof(str_temp), "G1 Z%.3f F%d\n", (double)(_StartZ-_Endpoint_of_thread), _FeedRate);
//             f->Write(str_temp);
//             //back to X start
//             SafeSnprintf(str_temp, sizeof(str_temp), "G0 X%.3f F%d\n", (double)_StartX, _FeedRate*2);
//             f->Write(str_temp);
//         }
//         f->Write("M5");
//         if (!f->Close())
//         {
//             reply.printf("G76 file not closed");
//             HandleReply(gb, GCodeResult::error, reply.c_str());
//             return true;
//         }
//         DoFileMacro(gb, _filename, true, code);
//         g76_active = false;
//         return true;
//     }
// }
///////////////////////////////////////////////////////////////////
//G83
GCodeResult GCodes::SetG83(GCodeBuffer& gb, const StringRef& reply) THROWS(GCodeException)
{
	if(machineType == MachineType::MillingMode || machineType == MachineType::HMCMode || machineType == MachineType::LatheMode){
        //pass
    }else{
        reply.copy("Change to Milling, HMC, or Lathe Mode First! ");
        return GCodeResult::error;
    }
    //
    if (gb.Seen('R'))
    {
        _Retractvalue = gb.GetFValue();
    }else{
        reply.copy("R parameter not found");
        return GCodeResult::error;
    }
    if (gb.Seen('Q'))
    {
        _Depth_of_each_peck = gb.GetFValue();
    }else{
        reply.copy("Q parameter not found");
        return GCodeResult::error;
    }
    if (gb.Seen('Z'))
    {
        _Depth_of_hole = gb.GetFValue();
    }else{
        reply.copy("Z parameter not found");
        return GCodeResult::error;
    }
    //Y axis feedrate
    if (gb.Seen('F'))
    {
        _FeedRate = gb.GetIValue();
    }else{
        reply.copy("F parameter not found");
        return GCodeResult::error;
    }
    CloseGodeMultiple();
    g83_active = true;

    String<StringLength256> activeComm;
    char _filename[30] = "/macros/G83_Setting.g";
    FileStore * const f = platform.OpenSysFile(_filename, OpenMode::write);
    if (f == nullptr){
        reply.printf("G83_Setting file not closed");
        return GCodeResult::error;
    }
    if(machineType == MachineType::MillingMode || machineType == MachineType::HMCMode){
    	//activeComm.cat("M5000\n");
    	//activeComm.cat("G4 P500\n");
    }
    else if(machineType == MachineType::LatheMode){
        //activeComm.copy("M801\n");
        //activeComm.cat("G4 P500\n");
    }
    f->Write(activeComm.c_str());
    if (!f->Close())
    {
        reply.printf("G83_Setting file not closed");
        return GCodeResult::error;
    }
    const int code = gb.GetCommandNumber();
    DoFileMacro(gb, _filename, true, code);

    return GCodeResult::ok;
}
bool GCodes::RunG83(GCodeBuffer& gb, const StringRef& reply) THROWS(GCodeException)
{
	const int code = gb.GetCommandNumber();
    //write the entire file
    char _filename[20] = "/macros/G83.g";
    char str_temp[255];
    _sumQ = 0;
    _real_Z = 0;
    FileStore * const f = platform.OpenSysFile(_filename, OpenMode::write);
    if (f == nullptr)
    {
        reply.printf("G83 file not closed");
        HandleReply(gb, GCodeResult::error, reply.c_str());
        return true;
    }
    else
    {
    	if(machineType == MachineType::MillingMode || machineType == MachineType::HMCMode){
    		bool _flag = false;
    		float _startZ = 0.0;
    		//get Now Z position
    		for (size_t axis = 0; axis < numVisibleAxes; ++axis)
    		{
    			if(axisLetters[axis] == 'z' || axisLetters[axis] == 'Z'){
    				_startZ = HideNan(GetUserCoordinate(axis));
    				_flag = true;
    				break;
    			}
    		}
    		if(_flag == false){
    			reply.printf("G83 - You need set Z axis!");
    			HandleReply(gb, GCodeResult::error, reply.c_str());
    			return true;
    		}

    		//move x y
    		newX = -999;
    		newY = -999;
    		//move x y
    		if (gb.GetCommandLetter() == 'X'){
    			newX  = gb.GetModalCommandNumber();
    			//debugPrintf("newX-CommandLetter\n");
    		}
    		else if(gb.Seen('X')){
    			newX = gb.GetFValue();
    			//debugPrintf("newX-Seen\n");
    		}

    		if (gb.GetCommandLetter() == 'Y'){
    		    newY  = gb.GetModalCommandNumber();
    			//debugPrintf("newY-CommandLetter\n");
    		}
    		else if(gb.Seen('Y')){
    			newY = gb.GetFValue();
    			//debugPrintf("newY-Seen\n");
    		}

    		//debugPrintf("newX=%f, newY=%f,\n", newX, newY);
    		if( newX == -999 && newX == -999 ){
    			//pass
    		}else{
    			f->Write("G0");
    			if(newX != -999){
    				SafeSnprintf(str_temp, sizeof(str_temp), " X%.3f", (double)newX);
    				f->Write(str_temp);
    			}
    			if(newY != -999){
    				SafeSnprintf(str_temp, sizeof(str_temp), " Y%.3f", (double)newY);
    				f->Write(str_temp);
    			}
    			f->Write("\n");
    		}
    		//Move to R plan
    		float _EndZ = _Depth_of_hole;
    		float _depthofhole = 0.0;
    		int iDirection = (_EndZ > _Retractvalue) ? 1 : -1;
    		_sumQ = 0.0;
    		_depthofhole = fabsf(_EndZ - _Retractvalue);
    		_real_Z = _Retractvalue;
    		//start peck
    		while(_depthofhole > _sumQ){
    			//move Z to subend
    			SafeSnprintf(str_temp, sizeof(str_temp), "G0 Z%.3f\n", (double)_real_Z);
    			f->Write(str_temp);
    			f->Write("G4 P10\n");
    			//move Z to R - Q
    			_sumQ += _Depth_of_each_peck;
    			if(_depthofhole >= _sumQ)
    				_real_Z = _Retractvalue + _sumQ * iDirection;
    			else
    			{
    				_real_Z = _Retractvalue + _depthofhole * iDirection;
    			}
    			SafeSnprintf(str_temp, sizeof(str_temp), "G1 Z%.3f F%d\n", (double)_real_Z,_FeedRate);
    			if(_sumQ != 0){f->Write(str_temp);}
    			//back to R
    			SafeSnprintf(str_temp, sizeof(str_temp), "G0 Z%.3f\n", (double)_Retractvalue);
    			f->Write(str_temp);
    			f->Write("G4 P10\n");
    		}
    		SafeSnprintf(str_temp, sizeof(str_temp), "G0 Z%.3f\n", (double)_startZ);
    	}
    	else if(machineType == MachineType::LatheMode){
    		bool _flag = false;
    		float _startZ = 0.0;
    		//get Now Z position
    		for (size_t axis = 0; axis < numVisibleAxes; ++axis)
    		{
    			if(axisLetters[axis] == 'z' || axisLetters[axis] == 'Z'){
    				_startZ = HideNan(GetUserCoordinate(axis));
    				_flag = true;
    				break;
    			}
    		}
    		if(_flag == false){
    			reply.printf("G83 - You need set Z axis!");
    			HandleReply(gb, GCodeResult::error, reply.c_str());
    			return true;
    		}

    		//move x
    		newX = -999;

    		//move x
    		if (gb.GetCommandLetter() == 'X') newX  = gb.GetModalCommandNumber();
    		else if(gb.Seen('X')) newX = gb.GetFValue();

    		if(newX == -999){
    			//pass
    		}else{
    			f->Write("G0");
    			if(newX != -999){
    				SafeSnprintf(str_temp, sizeof(str_temp), " X%.3f", (double)newX);
    				f->Write(str_temp);
    			}
    			f->Write("\n");
    		}
    		//Move to R plan
    		float _EndZ = _Depth_of_hole;
    		float _depthofhole = 0.0;
    		int iDirection = (_EndZ > _Retractvalue) ? 1 : -1;
    		_sumQ = 0.0;
    		_depthofhole = fabsf(_EndZ - _Retractvalue);
    		_real_Z = _Retractvalue;
    		//start peck
    		while(_depthofhole > _sumQ){
    			//move Z to subend
    			SafeSnprintf(str_temp, sizeof(str_temp), "G0 Z%.3f\n", (double)_real_Z);
    			f->Write(str_temp);
    			f->Write("G4 P10\n");
    			//move Z to R - Q
    			_sumQ += _Depth_of_each_peck;
    			if(_depthofhole >= _sumQ)
    				_real_Z = _Retractvalue + _sumQ * iDirection;
    			else
    			{
    				_real_Z = _Retractvalue + _depthofhole * iDirection;
    			}
    			SafeSnprintf(str_temp, sizeof(str_temp), "G1 Z%.3f F%d\n", (double)_real_Z,_FeedRate);
    			if(_sumQ != 0){f->Write(str_temp);}
    			//back to R
    			SafeSnprintf(str_temp, sizeof(str_temp), "G0 Z%.3f\n", (double)_Retractvalue);
    			f->Write(str_temp);
    			f->Write("G4 P10\n");
    		}
    		SafeSnprintf(str_temp, sizeof(str_temp), "G0 Z%.3f\n", (double)_startZ);
    	}
        f->Write(str_temp);
        if (!f->Close())
        {
            reply.printf("G83 file not closed");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        DoFileMacro(gb, _filename, true, code);
        return true;
    }
}
///////////////////////////////////////////////////////////////////
//G84 
GCodeResult GCodes::SetG84(GCodeBuffer& gb, const StringRef& reply) THROWS(GCodeException)
{
    if(machineType == MachineType::MillingMode || machineType == MachineType::HMCMode || machineType == MachineType::LatheMode){
        //pass
    }else{
        reply.copy("Change to Milling, HMC, or Lathe Mode First! ");
        return GCodeResult::error;
    }
    //
    if (gb.Seen('R'))
    {
        _Retractvalue = gb.GetFValue();
    }else{
        reply.copy("R parameter not found");
        return GCodeResult::error;
    }
    if (gb.Seen('Q'))
    {
        _Depth_of_each_peck = gb.GetFValue();
    }else{
        reply.copy("Q parameter not found");
        return GCodeResult::error;
    }
    if (gb.Seen('Z'))
    {
        _Depth_of_hole = gb.GetFValue();
    }else{
        reply.copy("Z parameter not found");
        return GCodeResult::error;
    }
    //Pitch of thread
    if (gb.Seen('F'))
    {
        _pitch_of_thread = gb.GetFValue();
    }else{
        reply.copy("F parameter not found");
        return GCodeResult::error;
    } 
    //Spindle RPM
    if (gb.Seen('S'))
    {
        _spindleRPM = gb.GetIValue();
    }else{
        reply.copy("S parameter not found");
        return GCodeResult::error;
    }
    CloseGodeMultiple();
    g84_active = true;
    //
    String<StringLength256> activeComm;
    char _filename[30] = "/macros/G84_Setting.g";
    FileStore * const f = platform.OpenSysFile(_filename, OpenMode::write);
    if (f == nullptr){
        reply.printf("G84_Setting file not closed");
        return GCodeResult::error;
    }
    if(machineType == MachineType::MillingMode){
        activeComm.copy("G28 D\n");
        activeComm.cat("G4 P4000\n");
        activeComm.cat("M5001\n");
    }
    else if(machineType == MachineType::LatheMode){
    	activeComm.copy("G28 A\n");
    	//activeComm.copy("G92 C0\n");
        activeComm.cat("G4 P3000\n");
        activeComm.cat("M801\n");
    }
    f->Write(activeComm.c_str());
    //activeComm.cat("M889\n");
    if (!f->Close())
    {
    	reply.printf("G84_Setting file not closed");
        return GCodeResult::error;
    }
    const int code = gb.GetCommandNumber();
    DoFileMacro(gb, _filename, true, code);
	return GCodeResult::ok;
}
bool GCodes::RunG84(GCodeBuffer& gb, const StringRef& reply) THROWS(GCodeException)
{
	const int code = gb.GetCommandNumber();
    //write the entire file
    char _filename[20] = "/macros/G84.g";
    char str_temp[255];
    _sumQ = 0;
    _real_Z = 0;
    FileStore * const f = platform.OpenSysFile(_filename, OpenMode::write);
    if (f == nullptr)
    {
        reply.printf("G84 file not closed");
        HandleReply(gb, GCodeResult::error, reply.c_str());
        return true;
    }
    else
    {
        if(machineType == MachineType::MillingMode || machineType == MachineType::HMCMode){
        	//check datas
        	if(_pitch_of_thread == 0){
        		reply.printf("pitch of thread is 0");
        		HandleReply(gb, GCodeResult::error, reply.c_str());
        		return true;
        	}
        	//
        	bool _flag = false;
        	float _startZ = 0.0;
        	//get Now Z position
        	for (size_t axis = 0; axis < numVisibleAxes; ++axis)
        	{
        		if(axisLetters[axis] == 'z' || axisLetters[axis] == 'Z'){
        			_startZ = HideNan(GetUserCoordinate(axis));
        			_flag = true;
        			break;
        		}
        	}
        	if(_flag == false){
        		reply.printf("G76 - You need set Z axis!");
        		HandleReply(gb, GCodeResult::error, reply.c_str());
        		return true;
        	}
        	float _turns = 0;
        	newX = -999;
        	newY = -999;
        	//move x y

        	if (gb.GetCommandLetter() == 'X') newX  = gb.GetModalCommandNumber();
        	else if(gb.Seen('X')) newX = gb.GetFValue();
    		if (gb.GetCommandLetter() == 'Y') newY  = gb.GetModalCommandNumber();
    		else if(gb.Seen('Y')) newY = gb.GetFValue();

        	if( newX == -999 && newX == -999 ){
        		//pass
        	}else{
        		f->Write("G0");
        		if(newX != -999){
        			SafeSnprintf(str_temp, sizeof(str_temp), " X%.3f", (double)newX);
        			f->Write(str_temp);
        		}
        		if(newY != -999){
        			SafeSnprintf(str_temp, sizeof(str_temp), " Y%.3f", (double)newY);
        			f->Write(str_temp);
        		}
        		f->Write("\n");
        	}
        	//SETTING MILLING SPINDLE TO POS 0
        	float _EndZ = _Depth_of_hole;
        	float _depthofhole = 0.0;
        	int iDirection = (_EndZ > _Retractvalue) ? 1 : -1;
        	_sumQ = 0.0;
        	_depthofhole = fabsf(_EndZ - _Retractvalue);
        	_real_Z = _Retractvalue;
        	//move Z to R
        	SafeSnprintf(str_temp, sizeof(str_temp), "G0 Z%.3f D0\n", (double)_real_Z);
        	f->Write(str_temp);
        	f->Write("G4 P100\n");
        	//feedrate fix
        	_FeedRate = _spindleRPM * _pitch_of_thread;
        	//start peck
        	while(_depthofhole > _sumQ){
        		//move Z to R - Q
        		_sumQ += _Depth_of_each_peck;
        		if(_depthofhole >= _sumQ){
        			_turns = _sumQ/_pitch_of_thread;
        			_real_Z = _Retractvalue + _sumQ * iDirection;
        		}
        		else
        		{
        			_turns = _depthofhole/_pitch_of_thread;
        			_real_Z = _Retractvalue + _depthofhole * iDirection;
        		}
        		SafeSnprintf(str_temp, sizeof(str_temp), "G1 Z%.3f F%d D%.3f\n", (double)_real_Z, _FeedRate, (double)_turns*-1);
        		if(_sumQ != 0){
        			f->Write(str_temp);
        			f->Write("G4 P100\n");
        		}
        		//back to R
        		SafeSnprintf(str_temp, sizeof(str_temp), "G1 Z%.3f F%d D0\n", (double)_Retractvalue, _FeedRate);
        		f->Write(str_temp);
        	            f->Write("G4 P100\n");
        	}
        	SafeSnprintf(str_temp, sizeof(str_temp), "G0 Z%.3f\n", (double)_startZ);
        	f->Write(str_temp);
        	f->Write("G4 P10\n");
        }
        else if(machineType == MachineType::LatheMode){
        	//check datas
        	if(_pitch_of_thread == 0){
        		reply.printf("pitch of thread is 0");
        		HandleReply(gb, GCodeResult::error, reply.c_str());
        		return true;
        	}
        	//
        	bool _flag = false;
        	float _startZ = 0.0;
        	//get Now Z position
        	for (size_t axis = 0; axis < numVisibleAxes; ++axis)
        	{
        		if(axisLetters[axis] == 'z' || axisLetters[axis] == 'Z'){
        			_startZ = HideNan(GetUserCoordinate(axis));
        			_flag = true;
        			break;
        		}
        	}
        	if(_flag == false){
        		reply.printf("G76 - You need set Z axis!");
        		HandleReply(gb, GCodeResult::error, reply.c_str());
        		return true;
        	}
        	float _turns = 0;
        	newX = -999;
        	//newY = -999;
        	//move x
        	if (gb.GetCommandLetter() == 'X') newX  = gb.GetModalCommandNumber();
        	else if(gb.Seen('X')) newX = gb.GetFValue();

        	if( newX == -999){
        		//pass
        	}else{
        		f->Write("G0");
        		if(newX != -999){
        			SafeSnprintf(str_temp, sizeof(str_temp), " X%.3f", (double)newX);
        			f->Write(str_temp);
        		}
        		f->Write("\n");
        	}
        	//SETTING MILLING SPINDLE TO POS 0
        	float _EndZ = _Depth_of_hole;
        	float _depthofhole = 0.0;
        	int iDirection = (_EndZ > _Retractvalue) ? 1 : -1;
        	_sumQ = 0.0;
        	_depthofhole = fabsf(_EndZ - _Retractvalue);
        	_real_Z = _Retractvalue;
        	//move Z to R
        	SafeSnprintf(str_temp, sizeof(str_temp), "G0 Z%.3f C0\n", (double)_real_Z);
        	f->Write(str_temp);
        	f->Write("G4 P1000\n");
        	//feedrate fix
        	//_FeedRate = _spindleRPM  * _pitch_of_thread;
        	_FeedRate = _spindleRPM * 360;
        	//start peck
        	while(_depthofhole > _sumQ){
        		//move Z to R - Q
        		_sumQ += _Depth_of_each_peck;
        		if(_depthofhole >= _sumQ){
        			_turns = _sumQ/_pitch_of_thread;
        			_real_Z = _Retractvalue + _sumQ * iDirection;
        		}
        		else
        		{
        			_turns = _depthofhole/_pitch_of_thread;
        			_real_Z = _Retractvalue + _depthofhole * iDirection;
        		}
        		SafeSnprintf(str_temp, sizeof(str_temp), "G1 Z%.3f F%d C%.3f\n", (double)_real_Z, _FeedRate, (double)_turns*360);
        		if(_sumQ != 0){
        			f->Write(str_temp);
        			f->Write("G4 P100\n");
        		}
        		//back to R
        		SafeSnprintf(str_temp, sizeof(str_temp), "G1 Z%.3f F%d C0\n", (double)_Retractvalue, _FeedRate);
        		f->Write(str_temp);
        		f->Write("G4 P100\n");
        	}
        	SafeSnprintf(str_temp, sizeof(str_temp), "G0 Z%.3f\n", (double)_startZ);
        	f->Write(str_temp);
        	f->Write("G4 P10\n");
        }

        //
        if (!f->Close())
        {
            reply.printf("G84 file not closed");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        DoFileMacro(gb, _filename, true, code);
        return true;
    }
}

///////////////////////////////////////////////////////////////////
//Helical boring
GCodeResult GCodes::SetM460(GCodeBuffer& gb, const StringRef& reply) THROWS(GCodeException)
{
    if (gb.Seen('A'))
    {
        _A_diaofhole = gb.GetFValue();
    }else{
        reply.copy("A parameter not found");
        return GCodeResult::error;
    }
    if (gb.Seen('B'))
    {
        _B_diaofcutter = gb.GetFValue();
    }else{
        reply.copy("B parameter not found");
        return GCodeResult::error;
    }
    if (gb.Seen('C'))
    {
        _Depth_of_hole = gb.GetFValue();
    }else{
        reply.copy("C parameter not found");
        return GCodeResult::error;
    }    
    if (gb.Seen('E'))
    {
        _Depth_of_each_peck = gb.GetFValue();
    }else{
        reply.copy("E parameter not found");
        return GCodeResult::error;
    }
    if (gb.Seen('F'))
    {
        _FeedRate = gb.GetIValue();
    }else{
        reply.copy("F parameter not found");
        return GCodeResult::error;
    }
    if (gb.Seen('S'))
    {
        _spindleRPM = gb.GetIValue();
    }else{
        reply.copy("S parameter not found");
        return GCodeResult::error;
    }
    if (gb.Seen('H'))
    {
        _CW_CCW = gb.GetIValue();
        if(_CW_CCW < 0 || _CW_CCW > 1 )_CW_CCW=0;
    }
    if(_B_diaofcutter > _A_diaofhole){
        reply.copy("Tool diameter parameter is bigger than pocket diameter");
        return GCodeResult::error;
    }
	return GCodeResult::ok;
}
bool GCodes::RunM460(GCodeBuffer& gb, const StringRef& reply) THROWS(GCodeException)
{
    const int code = gb.GetCommandNumber();
    //write the entire file
    char _filename[20] = "/macros/M460.g";
    char str_temp[255];
    _sumQ = 0;
    _real_Z = 0;
    FileStore * const f = platform.OpenSysFile(_filename, OpenMode::write);
    if (f == nullptr)
    {
        reply.printf("M460 file not closed");
        HandleReply(gb, GCodeResult::error, reply.c_str());
        return true;
    }
    else
    {   
        bool _flag = false;
        float _startZ = 0;
        //get Now Z position
        for (size_t axis = 0; axis < numVisibleAxes; ++axis)
        {
            if(axisLetters[axis] == 'z' || axisLetters[axis] == 'Z'){
                _real_Z = HideNan(GetUserCoordinate(axis));
                _flag = true;
                break;
            }
        }
        if(_flag == false){
            reply.printf("M460 - You need set Z axis!");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        //get Now X position
        _flag = false;
        for (size_t axis = 0; axis < numVisibleAxes; ++axis)
        {
            if(axisLetters[axis] == 'x' || axisLetters[axis] == 'X'){
                newX = HideNan(GetUserCoordinate(axis));
                _flag = true;
                break;
            }
        }
        if(_flag == false){
            reply.printf("M460 - You need set X axis!");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        //get Now Y position
        _flag = false;
        for (size_t axis = 0; axis < numVisibleAxes; ++axis)
        {
            if(axisLetters[axis] == 'y' || axisLetters[axis] == 'Y'){
                newY = HideNan(GetUserCoordinate(axis));
                _flag = true;
                break;
            }
        }
        if(_flag == false){
            reply.printf("M460 - You need set Y axis!");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        float _center_x = newX, _center_y = newY ;
        //run spindle
        SafeSnprintf(str_temp, sizeof(str_temp), "M3 S%d\n", _spindleRPM);
        f->Write(str_temp);
        //first pitch
        _startZ = _real_Z;
         _real_Z -= _Retractvalue;
        //start peck
        _sumQ = 0;
        //work radius
        float work_radius = 0;
        bool b_active_spiral = false;
        work_radius = _B_diaofcutter/2 <=0 ? 1 : _B_diaofcutter/2 ;
        b_active_spiral = true;
        _Depth_of_hole = fabsf(_Depth_of_hole);
        _Depth_of_each_peck = fabsf(_Depth_of_each_peck);
        // if( (_A_diaofhole/2) > _B_diaofcutter ){
        //     work_radius = _B_diaofcutter/2;
        //     b_active_spiral = true;
        // }else{
        //     work_radius = (_B_diaofcutter/2)-(_A_diaofhole/2);
        //     b_active_spiral = false;
        // }
        while(_Depth_of_hole > _sumQ){
            _sumQ += _Depth_of_each_peck;
            if(_Depth_of_hole > _sumQ){
                //helical
                SafeSnprintf(str_temp, sizeof(str_temp), "G1 X%.3f Y%.3f Z%.3f F%d\n", (double)_center_x, (double)_center_y, (double)_real_Z, _FeedRate);
                f->Write(str_temp);
                _real_Z -= _Depth_of_each_peck;
                SafeSnprintf(str_temp, sizeof(str_temp), "G2 X%.3f Y%.3f Z%.3f F%d I%.3f\n", (double)_center_x-work_radius, (double)_center_y, (double)_real_Z, _FeedRate,(double) work_radius);
                f->Write(str_temp);
                // SafeSnprintf(str_temp, sizeof(str_temp), "M2000 X%.3f Y%.3f R%.3f F%d E%.3f N%d H%d\n", (double)_center_x, (double)_center_y, (double)work_radius, _FeedRate, (double)_Depth_of_each_peck,360,_CW_CCW);
                // f->Write(str_temp);
            }
            else
            {
                //finalRadius = _Depth_of_hole - (_sumQ - _Depth_of_each_peck);
                _real_Z -= _Depth_of_hole - (_sumQ - _Depth_of_each_peck);
                _real_Z -= _Depth_of_each_peck;
                SafeSnprintf(str_temp, sizeof(str_temp), "G2 X%.3f Y%.3f Z%.3f F%d I%.3f\n", (double)_center_x-work_radius, (double)_center_y, (double)_real_Z, _FeedRate,(double) work_radius);
                f->Write(str_temp);
                SafeSnprintf(str_temp, sizeof(str_temp), "G2 X%.3f Y%.3f Z%.3f F%d I%.3f\n", (double)_center_x-work_radius, (double)_center_y, (double)_real_Z, _FeedRate,(double) work_radius);
                f->Write(str_temp);

                // SafeSnprintf(str_temp, sizeof(str_temp), "M2000 X%.3f Y%.3f R%.3f F%d E%.3f N%d H%d\n", (double)_center_x, (double)_center_y, (double)work_radius, _FeedRate, (double)finalRadius,360,_CW_CCW);
                // f->Write(str_temp);
                // //finish cycle
                // SafeSnprintf(str_temp, sizeof(str_temp), "M2000 X%.3f Y%.3f R%.3f F%d E%.3f N%d H%d\n", (double)_center_x, (double)_center_y, (double)work_radius, _FeedRate, (double)0,360,_CW_CCW);
                // f->Write(str_temp);
                //end
            }
            //Helical
            ////////spiral
            //stepover
            if(b_active_spiral){
                f->Write(";Spiral start\n");                
                //spiral start
                SafeSnprintf(str_temp, sizeof(str_temp), "M2001 X%.3f Y%.3f A%.3f B%.3f C%.3f F%d R%.3f P%.3f H%d\n", (double)_center_x, (double)_center_y, (double)_A_diaofhole, (double)_B_diaofcutter, (double)work_radius, _FeedRate, (double)1.0, (double)1.0 ,_CW_CCW);    
                f->Write(str_temp);
                //finish cycle
                SafeSnprintf(str_temp, sizeof(str_temp), "G2 X%.3f Y%.3f Z%.3f F%d I%.3f\n", (double)_center_x-((_A_diaofhole/2)-(_B_diaofcutter/2)), (double)_center_y, (double)_real_Z, _FeedRate,(double) ((_A_diaofhole/2)-(_B_diaofcutter/2)));
                f->Write(str_temp);
                // SafeSnprintf(str_temp, sizeof(str_temp), "M2000 X%.3f Y%.3f R%.3f F%d E%.3f N%d H%d\n", (double)_center_x, (double)_center_y, (double)((_A_diaofhole/2)-(_B_diaofcutter/2)), _FeedRate, (double)0,360,_CW_CCW);
                // f->Write(str_temp);               
                f->Write(";Spiral end\n");
            }            
        }
        f->Write("M5\n");
        //
        if (!f->Close())
        {
            reply.printf("M460 file not closed");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        DoFileMacro(gb, _filename, true, code);
        return true;
    }
}
//////////////////////////////////////////////////////////////////
bool GCodes::RunM799(GCodeBuffer& gb, const StringRef& reply) THROWS(GCodeException)
{
	if(bM799_1){
		StartTime = millis();
		//debugPrintf("StartTime=%d\n",StartTime);
		if (gb.Seen('P')){
			_timeout = gb.GetIValue();
			MessageType type = Aux2Message;
			String<StringLength256> activeComm;
			activeComm.copy(":S UNLOCK\n");
			platform.Message(type, activeComm.c_str());
		}
		else{
            reply.copy("P parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
		}
	}
	bM799_1 = false;
	if(IsLock){
		NowTime = millis();
		//debugPrintf("NowTime=%d\n",NowTime);
		//Check timeout
		if((NowTime - StartTime) >= _timeout){
			if(bM799_2){
				MessageType type = Aux2Message;
				String<StringLength256> activeComm;
				activeComm.copy(":S IDLETOOL\n");
				platform.Message(type, activeComm.c_str());
			}
			bM799_2 = false;
			if (!gb.DoDwellTime(2000))		// wait a second to allow the response to be sent back to the web server, otherwise it may retry
			{
				return false;
			}
			//debugPrintf("Timeout\n");
			bM799_1 = true;
			bM799_2 = true;
			const char* info = "System Timeout!";
			gb.SetState(GCodeState::abortWhenMovementFinished);
			gb.LatestMachineState().SetError(info);
			IsLock = true;
			return true;
		}
		if (!gb.DoDwellTime(1000))		// wait a second to allow the response to be sent back to the web server, otherwise it may retry
		{
			return false;
		}
//		MessageType type = Aux2Message;
//		String<StringLength256> activeComm;
//		activeComm.copy(":S UNLOCK\n");
//		platform.Message(type, activeComm.c_str());
		return false;
	}
	//Unlock
	else{
		MessageType type = Aux2Message;
		String<StringLength256> activeComm;
		activeComm.copy(":S IDLETOOL\n");
		platform.Message(type, activeComm.c_str());
		//debugPrintf("End\n");
		bM799_1 = true;
		return true;
	}
}
bool GCodes::RunM1989(GCodeBuffer& gb, const StringRef& reply) THROWS(GCodeException){
	if(nowMillToolNumber == reprap.GetCurrentToolNumber()){
		char info[255] = "Tool";
		SafeSnprintf(info, sizeof(info),"%s%d is already in the spindle\n", info, nowMillToolNumber);
		gb.SetState(GCodeState::abortWhenMovementFinished);
		gb.LatestMachineState().SetError(info);
	}
	return true;
}
void GCodes::RunM1992(GCodeBuffer& gb, const StringRef& reply) THROWS(GCodeException){
	char str_temp[255] = "G10";
	SafeSnprintf(str_temp, sizeof(str_temp),"%s P%d Z0\n", str_temp, nowMillToolNumber);

	debugPrintf("Str = %s \n", str_temp);
	gb.PutAndDecode(str_temp);
	reply.printf("");
	HandleReply(gb, GCodeResult::ok, reply.c_str());
}
void GCodes::RunM1993(GCodeBuffer& gb, const StringRef& reply) THROWS(GCodeException){
	char str_temp[255] = "G10";
	SafeSnprintf(str_temp, sizeof(str_temp),"%s P%d Z%.3f\n", str_temp, nowMillToolNumber,nowToolLength);

	debugPrintf("Str = %s \n", str_temp);
	gb.PutAndDecode(str_temp);
	reply.printf("");
	HandleReply(gb, GCodeResult::ok, reply.c_str());
}
bool GCodes::RunM1994(GCodeBuffer& gb, const StringRef& reply) THROWS(GCodeException){
	int _value;
	//Library
	//_ToolDetect_Pre
	if (gb.Seen('P')){
		_value = gb.GetIValue();
		//WITHDRAW_TOOL
		if(_value){
			if(_ToolDetect_Pre) return true;
			else{
				char info[255] = "No tool detected in ATC pocket";
				SafeSnprintf(info, sizeof(info),"%s #%d\n", info, nowMillToolNumber);
				gb.SetState(GCodeState::abortWhenMovementFinished);
				gb.LatestMachineState().SetError(info);
				return true;
			}
		}
		//Deposit//
		else{
			if(!_ToolDetect_Pre) return true;
			else{
				char info[255] = "No space detected in ATC pocket";
				SafeSnprintf(info, sizeof(info),"%s #%d\n", info, previousMillToolNumber);
				gb.SetState(GCodeState::abortWhenMovementFinished);
				gb.LatestMachineState().SetError(info);
				return true;
			}
		}
	}
	else{
		reply.copy("P parameter not found");
	    HandleReply(gb, GCodeResult::error, reply.c_str());
	    return true;
	}
	return true;
}
bool GCodes::RunM1995(GCodeBuffer& gb, const StringRef& reply) THROWS(GCodeException){
	const char*  filename;
	MessageType type = GenericMessage;
	String<StringLength256> activeComm;
	//Check door status
	//_DoorOpen_Pre, _DoorClose_Pre
	if(_DoorOpen_Pre == true && _DoorClose_Pre == false){
		//Check Milling tool status
		//_ToolEmpty_Pre(1), _ToolClamp_Pre(1), _ToolRelease_Pre(0)
		if(_ToolEmpty_Pre == true){
			//do withdraw
			//activeComm.copy("Withdraw tool\n");
			//platform.Message(type, activeComm.c_str());

			//find out macros
			switch(nowMillToolNumber){
				case 10:
					filename = "/macros/ATC/ATC_WITHDRAW_TOOL_10.g";
					break;

				case 11:
					filename = "/macros/ATC/ATC_WITHDRAW_TOOL_11.g";
					break;

				case 12:
					filename = "/macros/ATC/ATC_WITHDRAW_TOOL_12.g";
					break;

				case 13:
					filename = "/macros/ATC/ATC_WITHDRAW_TOOL_13.g";
					break;

				case 14:
					filename = "/macros/ATC/ATC_WITHDRAW_TOOL_14.g";
					break;

				case 15:
					filename = "/macros/ATC/ATC_WITHDRAW_TOOL_15.g";
					break;

				case 16:
					filename = "/macros/ATC/ATC_WITHDRAW_TOOL_16.g";
					break;

				case 17:
					filename = "/macros/ATC/ATC_WITHDRAW_TOOL_17.g";
					break;

				case 18:
					filename = "/macros/ATC/ATC_WITHDRAW_TOOL_18.g";
					break;

				case 19:
					filename = "/macros/ATC/ATC_WITHDRAW_TOOL_19.g";
					break;

				case 20:
					filename = "/macros/ATC/ATC_WITHDRAW_TOOL_20.g";
					break;

				case 21:
					filename = "/macros/ATC/ATC_WITHDRAW_TOOL_21.g";
					break;

				case 22:
					filename = "/macros/ATC/ATC_WITHDRAW_TOOL_22.g";
					break;

				case 23:
					filename = "/macros/ATC/ATC_WITHDRAW_TOOL_23.g";
					break;

				case 24:
					filename = "/macros/ATC/ATC_WITHDRAW_TOOL_24.g";
					break;

				case 25:
					filename = "/macros/ATC/ATC_WITHDRAW_TOOL_25.g";
					break;

				case 26:
					filename = "/macros/ATC/ATC_WITHDRAW_TOOL_26.g";
					break;

				case 27:
					filename = "/macros/ATC/ATC_WITHDRAW_TOOL_27.g";
					break;

				case 28:
					filename = "/macros/ATC/ATC_WITHDRAW_TOOL_28.g";
					break;

				case 29:
					filename = "/macros/ATC/ATC_WITHDRAW_TOOL_29.g";
					break;

				case 30:
					filename = "/macros/ATC/ATC_WITHDRAW_TOOL_30.g";
					break;

				case 31:
					filename = "/macros/ATC/ATC_WITHDRAW_TOOL_31.g";
					break;

				case 32:
					filename = "/macros/ATC/ATC_WITHDRAW_TOOL_32.g";
					break;

				case 33:
					filename = "/macros/ATC/ATC_WITHDRAW_TOOL_33.g";
					break;
			}
			DoFileMacro(gb, filename, true, 98);
		}
		//bypass
		else
		{
			activeComm.copy("No withdraw tool #%d\n",nowMillToolNumber);
			platform.Message(type, activeComm.c_str());
		}
	}
	else{
		const char* info = "No open door!";
		gb.SetState(GCodeState::abortWhenMovementFinished);
		gb.LatestMachineState().SetError(info);
		return true;
	}
	return true;
}
bool GCodes::RunM1996(GCodeBuffer& gb, const StringRef& reply) THROWS(GCodeException){
	const char*  filename;
	MessageType type = GenericMessage;
	String<StringLength256> activeComm;
	//Check door status
	//_DoorOpen_Pre, _DoorClose_Pre
	if(_DoorOpen_Pre == true && _DoorClose_Pre == false){
		//Check Milling tool status
		//_ToolEmpty_Pre(0), _ToolClamp_Pre(1), _ToolRelease_Pre(0)
		if(_ToolEmpty_Pre == false){
			//do depositing
			//activeComm.copy("Deposit tool\n");
			//platform.Message(type, activeComm.c_str());

			//find out macros
			switch(previousMillToolNumber){
				case 10:
					filename = "/macros/ATC/ATC_DEPOSIT_TOOL_10.g";
					break;

				case 11:
					filename = "/macros/ATC/ATC_DEPOSIT_TOOL_11.g";
					break;

				case 12:
					filename = "/macros/ATC/ATC_DEPOSIT_TOOL_12.g";
					break;

				case 13:
					filename = "/macros/ATC/ATC_DEPOSIT_TOOL_13.g";
					break;

				case 14:
					filename = "/macros/ATC/ATC_DEPOSIT_TOOL_14.g";
					break;

				case 15:
					filename = "/macros/ATC/ATC_DEPOSIT_TOOL_15.g";
					break;

				case 16:
					filename = "/macros/ATC/ATC_DEPOSIT_TOOL_16.g";
					break;

				case 17:
					filename = "/macros/ATC/ATC_DEPOSIT_TOOL_17.g";
					break;

				case 18:
					filename = "/macros/ATC/ATC_DEPOSIT_TOOL_18.g";
					break;

				case 19:
					filename = "/macros/ATC/ATC_DEPOSIT_TOOL_19.g";
					break;

				case 20:
					filename = "/macros/ATC/ATC_DEPOSIT_TOOL_20.g";
					break;

				case 21:
					filename = "/macros/ATC/ATC_DEPOSIT_TOOL_21.g";
					break;

				case 22:
					filename = "/macros/ATC/ATC_DEPOSIT_TOOL_22.g";
					break;

				case 23:
					filename = "/macros/ATC/ATC_DEPOSIT_TOOL_23.g";
					break;

				case 24:
					filename = "/macros/ATC/ATC_DEPOSIT_TOOL_24.g";
					break;

				case 25:
					filename = "/macros/ATC/ATC_DEPOSIT_TOOL_25.g";
					break;

				case 26:
					filename = "/macros/ATC/ATC_DEPOSIT_TOOL_26.g";
					break;

				case 27:
					filename = "/macros/ATC/ATC_DEPOSIT_TOOL_27.g";
					break;

				case 28:
					filename = "/macros/ATC/ATC_DEPOSIT_TOOL_28.g";
					break;

				case 29:
					filename = "/macros/ATC/ATC_DEPOSIT_TOOL_29.g";
					break;

				case 30:
					filename = "/macros/ATC/ATC_DEPOSIT_TOOL_30.g";
					break;

				case 31:
					filename = "/macros/ATC/ATC_DEPOSIT_TOOL_31.g";
					break;

				case 32:
					filename = "/macros/ATC/ATC_DEPOSIT_TOOL_32.g";
					break;

				case 33:
					filename = "/macros/ATC/ATC_DEPOSIT_TOOL_33.g";
					break;
			}
			DoFileMacro(gb, filename, true, 98);
		}
		//bypass
		else{
			//do depositing
			activeComm.copy("No deposit tool #%d\n",nowMillToolNumber);
			platform.Message(type, activeComm.c_str());
		}
	}
	else{
		const char* info = "No open door!";
		gb.SetState(GCodeState::abortWhenMovementFinished);
		gb.LatestMachineState().SetError(info);
		return true;
	}


	return true;
}
bool GCodes::RunM1998(GCodeBuffer& gb, const StringRef& reply) THROWS(GCodeException){
	if(reprap.GetCurrentToolNumber() >= 10 && reprap.GetCurrentToolNumber() <= 33){
		char _filename[30] = "/macros/ATC/MillTool.g";
		char str_temp[255];
		FileStore * const f = platform.OpenSysFile(_filename, OpenMode::write);
		nowMillToolNumber = reprap.GetCurrentToolNumber();
		SafeSnprintf(str_temp, sizeof(str_temp), "M1997 S%d \n",nowMillToolNumber);
		f->Write(str_temp);
//		SafeSnprintf(str_temp, sizeof(str_temp), "G10 P%d X0 Z0\n",nowMillToolNumber);
//		f->Write(str_temp);
//		SafeSnprintf(str_temp, sizeof(str_temp), "M563 P%d Z%.3f S\"Tool %d\"\n",reprap.GetCurrentToolNumber(),nowToolLength,reprap.GetCurrentToolNumber());
//		f->Write(str_temp);
//		SafeSnprintf(str_temp, sizeof(str_temp), "G10 P%d Z%.3f\n",reprap.GetCurrentToolNumber(),nowToolLength);
//		f->Write(str_temp);
		if (!f->Close())
		{
			reply.printf("MillTool file not closed");
		    HandleReply(gb, GCodeResult::error, reply.c_str());
		    return true;
		}
		DoFileMacro(gb, _filename, true, 98);
	}
	return true;
}

bool GCodes::CheckHomePosition(GCodeBuffer& gb, const StringRef& reply) THROWS(GCodeException)
{
	for(int i =0; i<6 ; i++){
		if(HomePosition[i] != 0){
			const char* info = "Home position error!";
			gb.SetState(GCodeState::abortWhenMovementFinished);
			gb.LatestMachineState().SetError(info);
		}
	}
	return true;
}

bool GCodes::WaitIOstatus(GCodeBuffer& gb, const StringRef& reply) THROWS(GCodeException)
{
	int _type;
	int _value;
	if (gb.Seen('P')){
		_type = gb.GetIValue();
	}
	else{
        reply.copy("P parameter not found");
        HandleReply(gb, GCodeResult::error, reply.c_str());
        return true;
	}
	if (gb.Seen('S')){
		_value = gb.GetIValue();
	}
	else{
		reply.copy("S parameter not found");
	    HandleReply(gb, GCodeResult::error, reply.c_str());
	    return true;
	}
	for(int i =0; i<5; i++){
		switch(i){
			//Door open detect
			case 0:
				if (gb.Seen('V')){

				}



				//Door close detect
				case 1:

					break;

				//No tool
				case 2:

					break;

				//Get tool
				case 3:

					break;

				//Release tool
				case 4:

					break;
			}
		}


	return true;
}

bool GCodes::GetIOstatus(GCodeBuffer& gb, const StringRef& reply) THROWS(GCodeException)
{
    int _value;
    int _GetBit;
	MessageType type = GenericMessage;
	String<StringLength256> activeComm;
	if (gb.Seen('I')){
		_value = gb.GetIValue();
		for(int i =0; i<6; i++){
			switch(i){
				//Door open detect//Library
				case 0:
					_DoorOpen = (_value & (1<<i));
					if(_DoorOpen_Pre != _DoorOpen){
						_DoorOpen_Pre = _DoorOpen;
//						if(_DoorOpen_Pre)activeComm.copy("DO1\n");
//						else activeComm.copy("DO0\n");
//						platform.Message(type, activeComm.c_str());
					}
					break;

				//Door close detect//Library
				case 1:
					_DoorClose = (_value & (1<<i));
					if(_DoorClose_Pre != _DoorClose){
						_DoorClose_Pre = _DoorClose;
//						if(_DoorClose_Pre)activeComm.copy("DC1\n");
//						else activeComm.copy("DC0\n");
//						platform.Message(type, activeComm.c_str());
					}
					break;

				//No tool//Milling
				case 2:
					_ToolEmpty = (_value & (1<<i));
					if(_ToolEmpty_Pre != _ToolEmpty){
						_ToolEmpty_Pre = _ToolEmpty;
//						if(_ToolEmpty_Pre)activeComm.copy("TE1\n");
//						else activeComm.copy("TE0\n");
//						platform.Message(type, activeComm.c_str());
					}
					break;

				//Get tool//Milling
				case 3:
					_ToolClamp = (_value & (1<<i));
					if(_ToolClamp_Pre != _ToolClamp){
						_ToolClamp_Pre = _ToolClamp;
//						if(_ToolClamp_Pre)activeComm.copy("TC1\n");
//						else activeComm.copy("TC0\n");
//						platform.Message(type, activeComm.c_str());
					}
					break;

				//Release tool//Milling
				case 4:
					_ToolRelease = (_value & (1<<i));
					if(_ToolRelease_Pre != _ToolRelease){
						_ToolRelease_Pre = _ToolRelease;
//						if(_ToolRelease_Pre)activeComm.copy("TR1\n");
//						else activeComm.copy("TR0\n");
//						platform.Message(type, activeComm.c_str());
					}
					break;

				//Tool detect//Library
				case 5:
					_ToolDetect = (_value & (1<<i));
					if(_ToolDetect_Pre != _ToolDetect){
							_ToolDetect_Pre = _ToolDetect;
//							if(_ToolDetect_Pre)activeComm.copy("TD1\n");
//							else activeComm.copy("TD0\n");
//							platform.Message(type, activeComm.c_str());
					}
					break;

			}
		}

    }
	else{
        reply.copy("I parameter not found");
        HandleReply(gb, GCodeResult::error, reply.c_str());
        return true;
	}
	return true;
}
//Circle & helical
bool GCodes::RunM2000(GCodeBuffer& gb, const StringRef& reply) THROWS(GCodeException)
{
    const int code = gb.GetCommandNumber();
    //write the entire file
    char _filename[20] = "/macros/M2000.g";
    char str_temp[255];
    _sumQ = 0;
    _real_Z = 0;
    FileStore * const f = platform.OpenSysFile(_filename, OpenMode::write);
    if (f == nullptr)
    {
        reply.printf("M2000 file not closed");
        HandleReply(gb, GCodeResult::error, reply.c_str());
        return true;
    }
    else
    {
        bool _flag = false;
        float _radius = 0 , _center_x = 0 , _center_y = 0, _start_Z =-999, _DepthOfHole =-999 , _angleNum = 360;
        //get Now X position
        for (size_t axis = 0; axis < numVisibleAxes; ++axis)
        {
            if(axisLetters[axis] == 'x' || axisLetters[axis] == 'X'){
                _center_x = HideNan(GetUserCoordinate(axis));
                _flag = true;
                break;
            }
        }
        if(_flag == false){
            reply.printf("M2000 - You need set X axis!");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        //get Now Y position
        for (size_t axis = 0; axis < numVisibleAxes; ++axis)
        {
            if(axisLetters[axis] == 'y' || axisLetters[axis] == 'Y'){
                _center_y = HideNan(GetUserCoordinate(axis));
                _flag = true;
                break;
            }
        }
        if(_flag == false){
            reply.printf("M2000 - You need set Y axis!");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        //get Now Z position
        for (size_t axis = 0; axis < numVisibleAxes; ++axis)
        {
            if(axisLetters[axis] == 'z' || axisLetters[axis] == 'Z'){
                _start_Z = HideNan(GetUserCoordinate(axis));
                _flag = true;
                break;
            }
        }
        if(_flag == false){
            reply.printf("M2000 - You need set Z axis!");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        //
        if (gb.Seen('X'))
        {
            _center_x = gb.GetFValue();
        } 
        //
        if (gb.Seen('Y'))
        {
            _center_y = gb.GetFValue();
        }
        //
        if (gb.Seen('R'))
        {
            _radius = gb.GetFValue();
        }else{
            reply.copy("R parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }       
        if (gb.Seen('F'))
        {
            _FeedRate = gb.GetIValue();
        }else{
            reply.copy("F parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        if (gb.Seen('E'))
        {
            _DepthOfHole = gb.GetFValue();
        }
        if (gb.Seen('N'))
        {
            _angleNum = gb.GetIValue();
            if(_angleNum < 3 || _angleNum > 9999 )_angleNum = 360;
        }
        _CW_CCW = 0;
        if (gb.Seen('H'))
        {
            _CW_CCW = gb.GetIValue();
            if(_CW_CCW < 0 || _CW_CCW > 1 )_CW_CCW=0;
        }
        //
        if( _DepthOfHole!=-999 ) _flag = true;
        float Z_of_step = _DepthOfHole / _angleNum;
        _real_Z = _start_Z;
        //Helical
        for(int i=0 ; i<_angleNum ; i++){
            if(_CW_CCW==1){
                newX = _radius * (float)cos(i * DegreesToRadians) + _center_x;
                newY = _radius * (float)sin(i * DegreesToRadians) + _center_y;
            }else{
                newX = _radius * (float)sin(i * DegreesToRadians) + _center_x;
                newY = _radius * (float)cos(i * DegreesToRadians) + _center_y;
            }
            if(_flag){
                _real_Z = _real_Z - Z_of_step;
                SafeSnprintf(str_temp, sizeof(str_temp), "G1 X%.3f Y%.3f Z%.3f F%d\n", (double)newX, (double)newY, (double)_real_Z, _FeedRate);
            }else{
                SafeSnprintf(str_temp, sizeof(str_temp), "G1 X%.3f Y%.3f F%d\n", (double)newX, (double)newY, _FeedRate);
            }
            f->Write(str_temp);
        }
        //
        if (!f->Close())
        {
            reply.printf("M2000 file not closed");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        DoFileMacro(gb, _filename, true, code);
        return true;
    }
}

///////////////////////////////////////////////////////////////////
//spiral
bool GCodes::RunM2001(GCodeBuffer& gb, const StringRef& reply) THROWS(GCodeException)
{
    const int code = gb.GetCommandNumber();
    //write the entire file
    char _filename[20] = "/macros/M2001.g";
    char str_temp[255];
    _sumQ = 0;
    _real_Z = 0;
    FileStore * const f = platform.OpenSysFile(_filename, OpenMode::write);
    if (f == nullptr)
    {
        reply.printf("M2001 file not closed");
        HandleReply(gb, GCodeResult::error, reply.c_str());
        return true;
    }
    else
    {
        float _center_x = 0 , _center_y = 0;
        float _offset_for_spiral = 0 , _resolution = 1 , _stepover = 1;
        bool _flag = false;
        //get Now X position
        for (size_t axis = 0; axis < numVisibleAxes; ++axis)
        {
            if(axisLetters[axis] == 'x' || axisLetters[axis] == 'X'){
                _center_x = HideNan(GetUserCoordinate(axis));
                _flag = true;
                break;
            }
        }
        if(_flag == false){
            reply.printf("M2001 - You need set X axis!");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        //get Now Y position
        for (size_t axis = 0; axis < numVisibleAxes; ++axis)
        {
            if(axisLetters[axis] == 'y' || axisLetters[axis] == 'Y'){
                _center_y = HideNan(GetUserCoordinate(axis));
                _flag = true;
                break;
            }
        }
        if(_flag == false){
            reply.printf("M2001 - You need set Y axis!");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }        
        //get value
        //
        if (gb.Seen('X'))
        {
            _center_x = gb.GetFValue();
        } 
        //
        if (gb.Seen('Y'))
        {
            _center_y = gb.GetFValue();
        }
        if (gb.Seen('A'))
        {
            _A_diaofhole = gb.GetFValue();
        }else{
            reply.copy("A parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }       
        if (gb.Seen('B'))
        {
            _B_diaofcutter = gb.GetFValue();
        }else{
            reply.copy("B parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        if (gb.Seen('C'))
        {
            _offset_for_spiral = gb.GetFValue();
        }

        if (gb.Seen('F'))
        {
            _FeedRate = gb.GetIValue();
        }else{
            reply.copy("F parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        if (gb.Seen('R'))
        {
            _resolution = gb.GetFValue();
        }else{
            reply.copy("R parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        if (gb.Seen('P'))
        {
            _stepover = gb.GetFValue();
        }
        _CW_CCW = 0;
        if (gb.Seen('H'))
        {
            _CW_CCW = gb.GetIValue();
            if(_CW_CCW < 0 || _CW_CCW > 1 )_CW_CCW=0;
        }
        float work_radius = _offset_for_spiral , _theta = 0;
        int _point_per_turns=5;
        int _count = 0;
        //spiral
        //_resolution
        float _dis_per_turns = _stepover*(1/(2*M_PI));        
        while( (_A_diaofhole/2) - work_radius > (_B_diaofcutter/2) ){
            
            _theta += 2*M_PI/_point_per_turns;
            work_radius = _offset_for_spiral + _theta * _dis_per_turns;
            if(_count % _point_per_turns == 0){
                _point_per_turns = round(2*M_PI*work_radius /_resolution);
                _count = 0 ;
            }
            if( (_A_diaofhole/2) - work_radius <= (_B_diaofcutter/2) ){
                f->Write(";Spiral inert break\n");
                break;
            }
            if(_CW_CCW==1){
                newX = work_radius * (float)cos(_theta) + _center_x;
                newY = work_radius * (float)sin(_theta) + _center_y;
            }else{
                newX = work_radius * (float)sin(_theta) + _center_x;
                newY = work_radius * (float)cos(_theta) + _center_y;
            }
            SafeSnprintf(str_temp, sizeof(str_temp), "G1 X%.3f Y%.3f F%d\n", (double)newX, (double)newY, _FeedRate);
            f->Write(str_temp);
            _count++;
        }
        //smooth leave
        // if(_CW_CCW==1){
        //     newX = work_radius * 0.9 * (float)cos(_theta) + _center_x;
        //     newY = work_radius * 0.9 * (float)sin(_theta) + _center_y;
        // }else{
        //     newX = work_radius * 0.9 * (float)sin(_theta) + _center_x;
        //     newY = work_radius * 0.9 * (float)cos(_theta) + _center_y;
        // }
        // SafeSnprintf(str_temp, sizeof(str_temp), "G1 X%.3f Y%.3f F%d\n", (double)newX, (double)newY, _FeedRate);
        // f->Write(str_temp);
        //
        if (!f->Close())
        {
            reply.printf("M2001 file not closed");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        DoFileMacro(gb, _filename, true, code);
        return true;
    }
}
// End

///////////////////////////////////////////////////////////////////
//Gear hobbing
bool GCodes::RunM2002(GCodeBuffer& gb, const StringRef& reply) THROWS(GCodeException)
{
    if(machineType == MachineType::MillingMode || machineType == MachineType::HMCMode){
        //pass
    }else{
        reply.printf("Change to Milling Mode First! ");
        HandleReply(gb, GCodeResult::error, reply.c_str());
        return true;
    }
    const int code = gb.GetCommandNumber();
    //write the entire file
    char _filename[20] = "/macros/M2002.g";
    char str_temp[255];
    
    FileStore * const f = platform.OpenSysFile(_filename, OpenMode::write);
    if (f == nullptr)
    {
        reply.printf("M2002 file not closed");
        HandleReply(gb, GCodeResult::error, reply.c_str());
        return true;
    }
    else
    {
        _sumQ = 0;
        _real_Z = 0;
        float __startY = 0 , __endY = 0, __startX = 0, __endX = 0, __pitch = 0, __MillingRPM = 0, __HeadRPM = 0;
             
        //get value
        //
        if (gb.Seen('Y'))
        {
            __startY = gb.GetFValue();
        }else{
            reply.copy("Y parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        //
        if (gb.Seen('Q'))
        {
            __endY = gb.GetFValue();
        }else{
            reply.copy("Q parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        if (gb.Seen('X'))
        {
            __startX = gb.GetFValue();
        }else{
            reply.copy("X parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }       
        if (gb.Seen('Z'))
        {
            __endX = gb.GetFValue();
        }else{
            reply.copy("Z parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        if (gb.Seen('P'))
        {
            __pitch = gb.GetFValue();
        }else{
            reply.copy("P parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }

        if (gb.Seen('S'))
        {
            __MillingRPM = gb.GetIValue();
        }else{
            reply.copy("S parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        if (gb.Seen('F'))
        {
            __HeadRPM = gb.GetIValue();
        }else{
            reply.copy("F parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        float __depthOfthread = fabsf(__endY - __startY);
        float __sumY = 0;
        //millingstock rpm
        // SafeSnprintf(str_temp, sizeof(str_temp), "M3 S%d \n", (double)newX, (double)newY, _FeedRate);
        // f->Write(str_temp);
        // //headstock rpm
        // while( __depthOfthread > __sumY ){
        //     SafeSnprintf(str_temp, sizeof(str_temp), "G1 X%.3f Y%.3f F%d\n", (double)newX, (double)newY, _FeedRate);
        //     f->Write(str_temp);
        //     _theta += 2*M_PI/_point_per_turns;
        //     work_radius = _offset_for_spiral + _theta * _dis_per_turns;
        //     if(_count % _point_per_turns == 0){
        //         _point_per_turns = round(2*M_PI*work_radius /_resolution);
        //         _count = 0 ;
        //     }
        //     if( (_A_diaofhole/2) - work_radius <= (_B_diaofcutter/2) ){
        //         f->Write(";Spiral inert break\n");
        //         break;
        //     }
        //     if(_CW_CCW==1){
        //         newX = work_radius * (float)cos(_theta) + _center_x;
        //         newY = work_radius * (float)sin(_theta) + _center_y;
        //     }else{
        //         newX = work_radius * (float)sin(_theta) + _center_x;
        //         newY = work_radius * (float)cos(_theta) + _center_y;
        //     }
        //     SafeSnprintf(str_temp, sizeof(str_temp), "G1 X%.3f Y%.3f F%d\n", (double)newX, (double)newY, _FeedRate);
        //     f->Write(str_temp);
        //     __sumY += __pitch;
        // }
        //
        if (!f->Close())
        {
            reply.printf("M2002 file not closed");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        //DoFileMacro(gb, _filename, true, code);
        return true;
    }
}

///////////////////////////////////////////////////////////////////
//Turning Cycle
bool GCodes::RunM2003(GCodeBuffer& gb, const StringRef& reply) THROWS(GCodeException)
{
    if(machineType != MachineType::LatheMode){
        reply.printf("Change to Lathe Mode First!");
        HandleReply(gb, GCodeResult::error, reply.c_str());
        return true;
    }
    const int code = gb.GetCommandNumber();
    //write the entire file
    char _filename[20] = "/macros/M2003.g";
    String<StringLength256> activeComm;
    FileStore * const f = platform.OpenSysFile(_filename, OpenMode::write);
    if (f == nullptr)
    {
        reply.printf("M2003 file not closed");
        HandleReply(gb, GCodeResult::error, reply.c_str());
        return true;
    }
    else
    {
        _real_Z = 0;
        float __startZ = 0 , __endZ = 0, __startX = 0, __endX = 0, __depthofcut = 0 , __spindleRPM = 0, __Feedrate = 0;
             
        //get value
        if (gb.Seen('X'))
        {
            __startX = gb.GetFValue();
        }else{
            reply.copy("X parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        //
        if (gb.Seen('E'))
        {
            __endX = gb.GetFValue();
        }else{
            reply.copy("E parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        //
        if (gb.Seen('Z'))
        {
            __startZ = gb.GetFValue();
        }else{
            reply.copy("Z parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        //
        if (gb.Seen('Q'))
        {
            __endZ = gb.GetFValue();
        }else{
            reply.copy("Q parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }     
        
        if (gb.Seen('P'))
        {
            __depthofcut = gb.GetFValue();
        }else{
            reply.copy("P parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }

        if (gb.Seen('F'))
        {
            __Feedrate = gb.GetIValue();
        }else{
            reply.copy("F parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }

        if (gb.Seen('S'))
        {
            __spindleRPM = gb.GetIValue();
        }else{
            reply.copy("S parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }

        // float Finishing_Pass = 0.0;
        // if (gb.Seen('U'))
        // {
        //     Finishing_Pass = gb.GetFValue();
        // }else{
        //     reply.copy("U parameter not found");
        //     HandleReply(gb, GCodeResult::error, reply.c_str());
        //     return true;
        // }
        // float Finishing_Feedrate = 0.0;
        // if (gb.Seen('V'))
        // {
        //     Finishing_Feedrate = gb.GetFValue();
        // }else{
        //     reply.copy("V parameter not found");
        //     HandleReply(gb, GCodeResult::error, reply.c_str());
        //     return true;
        // }
        
        //
        //__endX -= Finishing_Pass;
        int iDirection = (__endX > __startX) ? 1 : -1;
        float __sumX = 0.0 , _real_X = 0.0;
        float __DepthOfX = fabsf(__startX - __endX);
        activeComm.copy("");
        activeComm.catf("M3 S%.3f\n", (double)__spindleRPM);
        activeComm.cat("G4 P500\n");
        activeComm.catf("G0 Z%.3f X%.3f\n", (double)__startZ, (double)__startX);
        f->Write(activeComm.c_str());
        while(__DepthOfX > __sumX){
            activeComm.copy("");
            __sumX += __depthofcut;
            if(__DepthOfX >= __sumX){                
                _real_X = __startX + __sumX * iDirection;
            }
            else{
                _real_X = __startX + __DepthOfX * iDirection;
            }
            //to Z start
            activeComm.catf("G0 Z%.3f\n", (double)__startZ);
            //to X start
            activeComm.catf("G1 X%.3f F%.3f\n", (double)_real_X, (double)__Feedrate);
            activeComm.cat("G4 P10\n");
            //rotation
            activeComm.catf("G1 Z%.3f F%.3f\n", (double)__endZ, (double)__Feedrate);
            activeComm.cat("G4 P10\n");
            //back to X start
            activeComm.catf("G0 X%.3f\n", (double)__startX);
            f->Write(activeComm.c_str());
        }
        //Finishing_Pass
        // {
        //     //to Z start
        //     activeComm.catf("G0 Z%.3f\n", (double)__startZ);
        //     //to X start
        //     activeComm.catf("G1 X%.3f F%.3f\n", (double)__endX+Finishing_Pass, (double)Finishing_Feedrate);
        //     activeComm.cat("G4 P10\n");
        //     //rotation
        //     activeComm.catf("G1 Z%.3f F%.3f\n", (double)__endZ, (double)Finishing_Feedrate);
        //     activeComm.cat("G4 P10\n");
        //     //back to X start
        //     activeComm.catf("G0 X%.3f\n", (double)__startX);
        //     f->Write(activeComm.c_str());
        // }
        //end leave
        activeComm.copy("");
        activeComm.catf("G0 Z%.3f X%.3f\n", (double)__startZ, (double)__startX);
        activeComm.catf("M5 S\n");
        f->Write(activeComm.c_str());
        if (!f->Close())
        {
            reply.printf("M2003 file not closed");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        DoFileMacro(gb, _filename, true, code);
        return true;
    }
}
// End

///////////////////////////////////////////////////////////////////
//Facing Cycle
bool GCodes::RunG72(GCodeBuffer& gb, const StringRef& reply) THROWS(GCodeException)
{
    if(machineType != MachineType::LatheMode){
        reply.printf("Change to Lathe Mode First!");
        HandleReply(gb, GCodeResult::error, reply.c_str());
        return true;
    }
    const int code = gb.GetCommandNumber();
    //write the entire file
    char _filename[20] = "/macros/G72.g";
    String<StringLength256> activeComm;
    FileStore * const f = platform.OpenSysFile(_filename, OpenMode::write);
    if (f == nullptr)
    {
        reply.printf("G72 file not closed");
        HandleReply(gb, GCodeResult::error, reply.c_str());
        return true;
    }
    else
    {
        float __spindleRPM=0.0,__depthofcut =0.0 , __retractvalue =0.0 , __startZ = 0 , __endZ = 0,__startX = 0, __endX = 0, __Feedrate = 0;
        //get value
        if (gb.Seen('W'))
        {
            __depthofcut = gb.GetFValue();
        }else{
            reply.copy("W parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        //get value
        if (gb.Seen('R'))
        {
            __retractvalue = gb.GetFValue();
        }else{
            reply.copy("R parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }

        //get value
        if (gb.Seen('P'))
        {
            __startZ = gb.GetFValue();
        }else{
            reply.copy("P parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }

        //get value
        if (gb.Seen('Q'))
        {
            __endZ = gb.GetFValue();
        }else{
            reply.copy("Q parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }

        //get value
        if (gb.Seen('X'))
        {
            __startX = gb.GetFValue();
        }else{
            reply.copy("X parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        //
        if (gb.Seen('U'))
        {
            __endX = gb.GetFValue();
        }else{
            reply.copy("E parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        //
        if (gb.Seen('F'))
        {
            __Feedrate = gb.GetIValue();
        }else{
            reply.copy("F parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }

        if (gb.Seen('S'))
        {
            __spindleRPM = gb.GetIValue();
        }else{
            reply.copy("S parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }

        int iDirection = (__endZ > __startZ) ? 1 : -1;
        float __sumZ = 0.0 , _now_Z = 0.0;
        float __DepthOfZ = fabsf(__startZ - __endZ);
        activeComm.copy("");
        activeComm.catf("M3 S%.3f\n", (double)__spindleRPM);
        activeComm.cat("G4 P500\n");
        activeComm.catf("G0 Z%.3f X%.3f\n", (double)__startZ, (double)__startX);
        f->Write(activeComm.c_str());
        while(__DepthOfZ > __sumZ){
            activeComm.copy("");
            __sumZ += __depthofcut;
            if(__DepthOfZ >= __sumZ){                
                _now_Z = __startZ + __sumZ * iDirection;
            }
            else{
                _now_Z = __startZ + __DepthOfZ * iDirection;
            }
            //to Z start
            activeComm.catf("G0 Z%.3f\n", (double)_now_Z);
            //to X start
            activeComm.catf("G1 X%.3f F%.3f\n", (double)__endX, (double)__Feedrate);
            activeComm.cat("G4 P10\n");
            //rotation
            activeComm.catf("G0 Z%.3f\n", (double)_now_Z - __sumZ * iDirection);
            activeComm.cat("G4 P10\n");
            //back to X start
            activeComm.catf("G0 X%.3f\n", (double)__startX);
            f->Write(activeComm.c_str());
        }
        //end leave
        activeComm.copy("");
        activeComm.catf("G0 Z%.3f X%.3f\n", (double)__startZ, (double)__startX);
        activeComm.catf("M5 S\n");
        f->Write(activeComm.c_str());
        if (!f->Close())
        {
            reply.printf("G72 file not closed");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        DoFileMacro(gb, _filename, true, code);
        return true;
    }
}
// End

///////////////////////////////////////////////////////////////////
//Milling Polygon
bool GCodes::RunM2004(GCodeBuffer& gb, const StringRef& reply) THROWS(GCodeException)
{
    if(machineType == MachineType::MillingMode || machineType == MachineType::HMCMode){
        //pass
    }else{
        reply.printf("Change to Milling Mode First! ");
        HandleReply(gb, GCodeResult::error, reply.c_str());
        return true;
    }
    const int code = gb.GetCommandNumber();
    //write the entire file
    char _filename[20] = "/macros/M2004.g";
    String<StringLength256> activeComm;
    FileStore * const f = platform.OpenSysFile(_filename, OpenMode::write);
    if (f == nullptr)
    {
        reply.printf("M2004 file not closed");
        HandleReply(gb, GCodeResult::error, reply.c_str());
        return true;
    }
    else
    {
        float __startZ = 0.0 , __endZ = 0.0, __startX = 0, __endX = 0, __depthofcut = 0 , __spindleRPM = 0, __Feedrate = 0;
        int __Side = 0;
        float __dimensionAcrossFlats = 0.0 , __diameterOfTool = 0.0 , __startY = 0.0, __endY = 0.0;

        //get value
        if (gb.Seen('Q'))
        {
            __Side = gb.GetIValue();
        }else{
            reply.copy("Q parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        if (gb.Seen('D'))
        {
            __dimensionAcrossFlats = gb.GetIValue();
        }else{
            reply.copy("D parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        if (gb.Seen('H'))
        {
            __diameterOfTool = gb.GetIValue();
        }else{
            reply.copy("H parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        if (gb.Seen('Y'))
        {
            __startY = gb.GetFValue();
        }else{
            reply.copy("Y parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        if (gb.Seen('E'))
        {
            __depthofcut = gb.GetFValue();
        }else{
            reply.copy("E parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        if (gb.Seen('X'))
        {
            __startX = gb.GetFValue();
        }else{
            reply.copy("X parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        //
        if (gb.Seen('N'))
        {
            __endX = gb.GetFValue();
        }else{
            reply.copy("N parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        //
        if (gb.Seen('Z'))
        {
            __startZ = gb.GetFValue();
        }else{
            reply.copy("Z parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        //
        float _startAngle = 0.0;
        if (gb.Seen('K'))
        {
            _startAngle = gb.GetFValue();
        }else{
            reply.copy("K parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        //
        if (gb.Seen('S'))
        {
            __spindleRPM = gb.GetIValue();
        }else{
            reply.copy("S parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        //
        if (gb.Seen('F'))
        {
            __Feedrate = gb.GetIValue();
        }else{
            reply.copy("F parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        //
        int imode = 0;
        if (gb.Seen('J'))
        {
            imode = gb.GetIValue();
        }else{
            reply.copy("J parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        //imode = (imode > 3) || (imode < 0) ? 0 : imode;
        //
       
        int iDirection = (0 > __startY) ? 1 : -1;
        __diameterOfTool = abs(__diameterOfTool / 2);
        __endY = fabsf(__dimensionAcrossFlats/2) + __diameterOfTool;
        if (__startY < 0) __endY *= -1;
        //
        float _turnAngle = abs(360 / __Side);
        //
        activeComm.copy("");
        activeComm.catf("M3 S%.3f\n", (double)__spindleRPM);
        activeComm.catf("G0 X%.3f\n", (double)__startX);
        activeComm.catf("G0 Y%.3f\n", (double)__startY);
        activeComm.catf("G0 Z%.3f\n", (double)__startZ);
        if(imode==1){
            activeComm.cat("M5 A\n");
            activeComm.cat("M800\n");
            activeComm.cat("M98 P\"/macros/0_SW_C_AXIS.g\"\n");
        }
        f->Write(activeComm.c_str());
        for(float i=0 ; i<360 ; i = _turnAngle + i){
            activeComm.copy("");
            if(imode==0){
                activeComm.cat("M801\n");
                activeComm.cat("G4 P500\n");
                activeComm.catf("M19 A%0.3f\n" , i + _startAngle );
                activeComm.cat("G4 P1000\n");
            }else if(imode==2){
                activeComm.cat("M801\n");
                activeComm.cat("G4 P500\n");
                activeComm.catf("M19 A%0.3f\n" , i + _startAngle );
                activeComm.cat("G4 P1000\n");
                activeComm.cat("M800\n");
            }else{
                activeComm.catf("G0 A%0.3f\n" , i + _startAngle );
            }
            
            activeComm.catf("M2005 X%.3f N%.3f Y%.3f D%.3f E%.3f Z%.3f S%.3f F%.3f\n"
            , (double)__startX, (double)__endX, (double)__startY, (double)__endY, (double)__depthofcut, (double)__startZ
            , (double)__spindleRPM, (double)__Feedrate);
            f->Write(activeComm.c_str());
        }
        
        //end leave
        activeComm.copy("");
        activeComm.catf("G0 Z%.3f\n", (double)__startZ);
        activeComm.catf("G0 X%.3f\n", (double)__startX);
        activeComm.catf("G0 Y%.3f\n", (double)__startY);
        activeComm.cat("M5 S\n");
        f->Write(activeComm.c_str());
        if (!f->Close())
        {
            reply.printf("M2004 file not closed");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        DoFileMacro(gb, _filename, true, code);
        return true;
    }
}
// End

///////////////////////////////////////////////////////////////////
//Milling Side Cycle
bool GCodes::RunM2005(GCodeBuffer& gb, const StringRef& reply) THROWS(GCodeException)
{
    if(machineType == MachineType::MillingMode || machineType == MachineType::HMCMode){
        //pass
    }else{
        reply.printf("Change to Milling Mode First! ");
        HandleReply(gb, GCodeResult::error, reply.c_str());
        return true;
    }
    const int code = gb.GetCommandNumber();
    //write the entire file
    char _filename[20] = "/macros/M2005.g";
    String<StringLength256> activeComm;
    FileStore * const f = platform.OpenSysFile(_filename, OpenMode::write);
    if (f == nullptr)
    {
        reply.printf("M2005 file not closed");
        HandleReply(gb, GCodeResult::error, reply.c_str());
        return true;
    }
    else
    {
        float __startX = 0.0 , __endX = 0.0;
        float __startY = 0.0 , __endY = 0.0;
        float __startZ = 0.0 , __depthofcut = 0 , __spindleRPM = 0, __Feedrate = 0;
        
        if (gb.Seen('X'))
        {
            __startX = gb.GetFValue();
        }else{
            reply.copy("X parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        if (gb.Seen('N'))
        {
            __endX = gb.GetFValue();
        }else{
            reply.copy("N parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        if (gb.Seen('Y'))
        {
            __startY = gb.GetFValue();
        }else{
            reply.copy("Y parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        if (gb.Seen('D'))
        {
            __endY = gb.GetFValue();
        }else{
            reply.copy("D parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        if (gb.Seen('E'))
        {
            __depthofcut = gb.GetFValue();
        }else{
            reply.copy("E parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        //
        if (gb.Seen('Z'))
        {
            __startZ = gb.GetFValue();
        }else{
            reply.copy("Z parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        if (gb.Seen('S'))
        {
            __spindleRPM = gb.GetIValue();
        }else{
            reply.copy("S parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        //
        if (gb.Seen('F'))
        {
            __Feedrate = gb.GetIValue();
        }else{
            reply.copy("F parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        //
        int iDirection = (0 > __startY) ? 1 : -1;
        float __sumY = 0.0 , _real_Y = 0.0;
        float __DepthOfY = fabsf(__startY - __endY);
        activeComm.copy("");
        activeComm.catf("M3 S%.3f\n", (double)__spindleRPM);
        activeComm.catf("G0 X%.3f Y%.3f\n", (double)__startX, (double)__startY);
        activeComm.catf("G0 Z%.3f\n", (double)__startZ);
        f->Write(activeComm.c_str());
        __sumY = -__depthofcut;
        while(__DepthOfY > __sumY){
            activeComm.copy("");
            __sumY += __depthofcut;
            if(__DepthOfY >= __sumY){                
                _real_Y = __startY + __sumY * iDirection;
            }
            else{
                _real_Y = __startY + __DepthOfY * iDirection;
            }
            //to X start
            activeComm.catf("G0 X%.3f\n", (double)__startX);
            //to Y start
            activeComm.catf("G0 Y%.3f\n", (double)_real_Y);
            //to X start
            activeComm.catf("G1 X%.3f F%.3f\n", (double)__endX, (double)__Feedrate);
            activeComm.cat("G4 P10\n");
            //back to Y start
            activeComm.catf("G0 Y%.3f\n", (double)_real_Y - __sumY * iDirection);
            f->Write(activeComm.c_str());
        }
        //end leave
        activeComm.copy("");
        activeComm.catf("G0 Z%.3f X%.3f\n", (double)__startZ, (double)__startX);
        //activeComm.catf("M5 S\n");
        f->Write(activeComm.c_str());
        if (!f->Close())
        {
            reply.printf("M2005 file not closed");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        DoFileMacro(gb, _filename, true, code);
        return true;
    }
}
// End

///////////////////////////////////////////////////////////////////
//Milling Facing Cycle
bool GCodes::RunM2006(GCodeBuffer& gb, const StringRef& reply) THROWS(GCodeException)
{
    if(machineType == MachineType::MillingMode || machineType == MachineType::HMCMode){
        //pass
    }else{
        reply.printf("Change to Milling Mode First! ");
        HandleReply(gb, GCodeResult::error, reply.c_str());
        return true;
    }
    const int code = gb.GetCommandNumber();
    //write the entire file
    char _filename[20] = "/macros/M2006.g";
    String<StringLength256> activeComm;
    FileStore * const f = platform.OpenSysFile(_filename, OpenMode::write);
    if (f == nullptr)
    {
        reply.printf("M2006 file not closed");
        HandleReply(gb, GCodeResult::error, reply.c_str());
        return true;
    }
    else
    {
        float __startX = 0.0 , __endX = 0.0;
        float __startY = 0.0 , __endY = 0.0;
        float __startZ = 0.0 , __endZ = 0.0, __depthofcut = 0 , __spindleRPM = 0, __Feedrate = 0;
        
        if (gb.Seen('X'))
        {
            __startX = gb.GetFValue();
        }else{
            reply.copy("X parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        if (gb.Seen('N'))
        {
            __endX = gb.GetFValue();
        }else{
            reply.copy("N parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        if (gb.Seen('Y'))
        {
            __startY = gb.GetFValue();
        }else{
            reply.copy("Y parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        if (gb.Seen('D'))
        {
            __endY = gb.GetFValue();
        }else{
            reply.copy("D parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        //
        if (gb.Seen('Z'))
        {
            __startZ = gb.GetFValue();
        }else{
            reply.copy("Z parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        if (gb.Seen('W'))
        {
            __endZ = gb.GetFValue();
        }else{
            reply.copy("W parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        if (gb.Seen('E'))
        {
            __depthofcut = gb.GetFValue();
        }else{
            reply.copy("E parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        if (gb.Seen('S'))
        {
            __spindleRPM = gb.GetIValue();
        }else{
            reply.copy("S parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        //
        if (gb.Seen('F'))
        {
            __Feedrate = gb.GetIValue();
        }else{
            reply.copy("F parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        //
        int iDirection = (__endX > __startX) ? 1 : -1;
        float __sumD = 0.0 , _real_D = 0.0;
        float __DepthOfD = fabsf(__endX - __startX);
        activeComm.copy("");
        activeComm.catf("M3 S%.3f\n", (double)__spindleRPM);
        activeComm.catf("G0 X%.3f Y%.3f\n", (double)__startX, (double)__startY);
        activeComm.catf("G0 Z%.3f\n", (double)__startZ);
        f->Write(activeComm.c_str());
        __sumD = -__depthofcut;
        while(__DepthOfD > __sumD){
            activeComm.copy("");
            __sumD += __depthofcut;
            if(__DepthOfD >= __sumD){                
                _real_D = __startX + __sumD * iDirection;
            }
            else{
                _real_D = __startX + __DepthOfD * iDirection;
            }
            //to X start
            activeComm.catf("G0 X%.3f\n", (double)_real_D);
            //to Y start
            activeComm.catf("G0 Y%.3f\n", (double)__startY);
            //to Z end
            activeComm.catf("G0 Z%.3f\n", (double)__endZ);
            //to Y end
            activeComm.catf("G1 Y%.3f F%.3f\n", (double)__endY, (double)__Feedrate);
            activeComm.cat("G4 P10\n");
            //back to Z start
            activeComm.catf("G0 Z%.3f\n", (double)__startZ);
            f->Write(activeComm.c_str());
        }
        //end leave
        activeComm.copy("");
        activeComm.catf("G0 Z%.3f\n", (double)__startZ);
        activeComm.catf("G0 X%.3f Y%.3f\n", (double)__startX, (double)__startY);
        //activeComm.catf("M5 S\n");
        f->Write(activeComm.c_str());
        if (!f->Close())
        {
            reply.printf("M2006 file not closed");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        DoFileMacro(gb, _filename, true, code);
        return true;
    }
}
// End
///////////////////////////////////////////////////////////////////
//Milling Polygon(Facing)
bool GCodes::RunM2007(GCodeBuffer& gb, const StringRef& reply) THROWS(GCodeException)
{
    if(machineType == MachineType::MillingMode || machineType == MachineType::HMCMode){
        //pass
    }else{
        reply.printf("Change to Milling Mode First! ");
        HandleReply(gb, GCodeResult::error, reply.c_str());
        return true;
    }
    const int code = gb.GetCommandNumber();
    //write the entire file
    char _filename[20] = "/macros/M2007.g";
    String<StringLength256> activeComm;
    FileStore * const f = platform.OpenSysFile(_filename, OpenMode::write);
    if (f == nullptr)
    {
        reply.printf("M2007 file not closed");
        HandleReply(gb, GCodeResult::error, reply.c_str());
        return true;
    }
    else
    {
        float  __depthofcut = 0.0, __spindleRPM = 0.0, __Feedrate = 0.0;
        int __Side = 0;
        float __startX = 0.0 , __endX = 0.0;
        float __startY = 0.0 , __endY = 0.0;
        float __startZ = 0.0 , __endZ = 0.0;
        float __dimensionAcrossFlats = 0.0 , __diameterOfTool = 0.0;

        //get value
        if (gb.Seen('Q'))
        {
            __Side = gb.GetIValue();
        }else{
            reply.copy("Q parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        if (gb.Seen('D'))
        {
            __dimensionAcrossFlats = gb.GetIValue();
        }else{
            reply.copy("D parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        if (gb.Seen('H'))
        {
            __diameterOfTool = gb.GetIValue();
        }else{
            reply.copy("H parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        if (gb.Seen('Y'))
        {
            __startY = gb.GetFValue();
        }else{
            reply.copy("Y parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        if (gb.Seen('I'))
        {
            __endY = gb.GetFValue();
        }else{
            reply.copy("I parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        if (gb.Seen('X'))
        {
            __startX = gb.GetFValue();
        }else{
            reply.copy("X parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        //
        if (gb.Seen('N'))
        {
            __endX = gb.GetFValue();
        }else{
            reply.copy("N parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        //
        if (gb.Seen('W'))
        {
            __depthofcut = gb.GetFValue();
        }else{
            reply.copy("W parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        //
        if (gb.Seen('Z'))
        {
            __startZ = gb.GetFValue();
        }else{
            reply.copy("Z parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        //
        float _startAngle = 0.0;
        if (gb.Seen('K'))
        {
            _startAngle = gb.GetFValue();
        }else{
            reply.copy("K parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        //
        if (gb.Seen('S'))
        {
            __spindleRPM = gb.GetIValue();
        }else{
            reply.copy("S parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        //
        if (gb.Seen('F'))
        {
            __Feedrate = gb.GetIValue();
        }else{
            reply.copy("F parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        //
        int imode = 0;
        if (gb.Seen('E'))
        {
            imode = gb.GetIValue();
        }else{
            reply.copy("E parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        imode = (imode > 1) || (imode < 0) ? 0 : imode;
        //
        int iDirection = (__endX > __startX) ? 1 : -1;
        __diameterOfTool = fabsf(__diameterOfTool);
        __dimensionAcrossFlats = fabsf(__dimensionAcrossFlats);
        __endZ = __dimensionAcrossFlats/2;
        __startX += (__diameterOfTool/2) * iDirection;
        __endX -= (__diameterOfTool/2) * iDirection;
        //
        float _turnAngle = abs(360 / __Side);
        //
        activeComm.copy("");
        activeComm.catf("M3 S%.3f\n", (double)__spindleRPM);
        activeComm.catf("G0 X%.3f\n", (double)__startX);
        activeComm.catf("G0 Y%.3f\n", (double)__startY);
        activeComm.catf("G0 Z%.3f\n", (double)__startZ);
        if(imode==1){
            activeComm.cat("M5 A\n");
            activeComm.cat("M800\n");
            activeComm.cat("M98 P\"/macros/0_SW_C_AXIS.g\"\n");
        }
        f->Write(activeComm.c_str());
        for(float i=0 ; i<360 ; i = _turnAngle + i){
            activeComm.copy("");
            if(imode==0){
                activeComm.cat("M801\n");
                activeComm.cat("G4 P500\n");
                activeComm.catf("M19 A%d\n" , i + _startAngle );
            }else if(imode==2){
                activeComm.cat("M801\n");
                activeComm.cat("G4 P500\n");
                activeComm.catf("M19 A%d\n" , i + _startAngle );
                activeComm.cat("G4 P700\n");
                activeComm.cat("M800\n");
            }else{
                activeComm.catf("G0 C%d\n" , i + _startAngle );
            }
            activeComm.catf("M2006 X%.3f N%.3f Y%.3f D%.3f Z%.3f W%.3f E%.3f S%.3f F%.3f\n"
            , (double)__startX, (double)__endX, (double)__startY, (double)__endY, (double)__startZ, (double)__endZ
            , (double)__depthofcut, (double)__spindleRPM, (double)__Feedrate);
            f->Write(activeComm.c_str());
        }
        
        //end leave
        activeComm.copy("");
        activeComm.catf("G0 Z%.3f\n", (double)__startZ);
        activeComm.catf("G0 X%.3f\n", (double)__startX);
        activeComm.catf("G0 Y%.3f\n", (double)__startY);
        activeComm.catf("M5 S\n");
        f->Write(activeComm.c_str());
        if (!f->Close())
        {
            reply.printf("M2007 file not closed");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        DoFileMacro(gb, _filename, true, code);
        return true;
    }
}
// End

///////////////////////////////////////////////////////////////////
//Grooving(for lathe)
bool GCodes::RunM2008(GCodeBuffer& gb, const StringRef& reply) THROWS(GCodeException)
{
    if(machineType != MachineType::LatheMode){
        reply.printf("Change to Lathe Mode First!");
        HandleReply(gb, GCodeResult::error, reply.c_str());
        return true;
    }
    const int code = gb.GetCommandNumber();
    //write the entire file
    char _filename[20] = "/macros/M2008.g";
    String<StringLength256> activeComm;
    FileStore * const f = platform.OpenSysFile(_filename, OpenMode::write);
    if (f == nullptr)
    {
        reply.printf("M2008 file not closed");
        HandleReply(gb, GCodeResult::error, reply.c_str());
        return true;
    }
    else
    {
        float  __depthofcut = 0.0, __spindleRPM = 0.0, __Feedrate = 0.0;
        float __startX = 0.0 , __endX = 0.0;
        float __startZ = 0.0;

        //
        if (gb.Seen('W'))
        {
            __depthofcut = gb.GetFValue();
        }else{
            reply.copy("W parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        //
        if (gb.Seen('X'))
        {
            __startX = gb.GetFValue();
        }else{
            reply.copy("X parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        //
        if (gb.Seen('U'))
        {
            __endX = gb.GetFValue();
        }else{
            reply.copy("U parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        //
        if (gb.Seen('Z'))
        {
            __startZ = gb.GetFValue();
        }else{
            reply.copy("Z parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        //
        if (gb.Seen('F'))
        {
            __Feedrate = gb.GetIValue();
        }else{
            reply.copy("F parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        //
        if (gb.Seen('S'))
        {
            __spindleRPM = gb.GetIValue();
        }else{
            reply.copy("S parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        
        //
        int iDirection = (__endX > __startX) ? 1 : -1;
        float __sumX = 0.0 , __nowX = 0.0 , __DepthOfX = 0.0;
        float __DepthOfD = fabsf(__endX - __startX);
        //
        activeComm.copy("");
        activeComm.catf("M3 S%.3f\n", (double)__spindleRPM);
        activeComm.catf("G0 X%.3f\n", (double)__startX);
        activeComm.catf("G0 Z%.3f\n", (double)__startZ);
        f->Write(activeComm.c_str());
        while(__DepthOfD > __sumX){
            activeComm.copy("");
            __sumX += __depthofcut;
            if(__DepthOfX >= __sumX){                
                __nowX = __startX + __sumX * iDirection;
            }
            else{
                __nowX = __startX + __DepthOfX * iDirection;
            }
            //to X start
            activeComm.catf("G1 X%.3f F%.3f\n", (double)__nowX, (double)__Feedrate);
            activeComm.cat("G4 P10\n");
            //back to X start
            activeComm.catf("G0 X%.3f\n", (double)__startX);
            f->Write(activeComm.c_str());
        }
        
        //end leave
        activeComm.copy("");
        activeComm.catf("G0 Z%.3f\n", (double)__startZ);
        activeComm.catf("G0 X%.3f\n", (double)__startX);
        activeComm.catf("M5 S\n");
        f->Write(activeComm.c_str());
        if (!f->Close())
        {
            reply.printf("M2008 file not closed");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        DoFileMacro(gb, _filename, true, code);
        return true;
    }
}
// End

///////////////////////////////////////////////////////////////////
//Drilling(for lathe)
bool GCodes::RunM2009(GCodeBuffer& gb, const StringRef& reply) THROWS(GCodeException)
{
    if(machineType != MachineType::LatheMode){
        reply.printf("Change to Lathe Mode First!");
        HandleReply(gb, GCodeResult::error, reply.c_str());
        return true;
    }
    const int code = gb.GetCommandNumber();
    //write the entire file
    char _filename[20] = "/macros/M2009.g";
    String<StringLength256> activeComm;
    FileStore * const f = platform.OpenSysFile(_filename, OpenMode::write);
    if (f == nullptr)
    {
        reply.printf("M2009 file not closed");
        HandleReply(gb, GCodeResult::error, reply.c_str());
        return true;
    }
    else
    {
        float  __depthofcut = 0.0, __spindleRPM = 0.0, __Feedrate = 0.0;
        float __startX = 0.0 , __endX = 0.0;
        float __startZ = 0.0 , __endZ = 0.0;

        //
        if (gb.Seen('Z'))
        {
            __startZ = gb.GetFValue();
        }else{
            reply.copy("Z parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        //
        if (gb.Seen('U'))
        {
            __endZ = gb.GetFValue();
        }else{
            reply.copy("U parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        //
        if (gb.Seen('X'))
        {
            __startX = gb.GetFValue();
        }else{
            reply.copy("X parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        //
        if (gb.Seen('Q'))
        {
            __depthofcut = gb.GetFValue();
        }else{
            reply.copy("Q parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        //
        if (gb.Seen('F'))
        {
            __Feedrate = gb.GetIValue();
        }else{
            reply.copy("F parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        //
        if (gb.Seen('S'))
        {
            __spindleRPM = gb.GetIValue();
        }else{
            reply.copy("S parameter not found");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        
        //
        int iDirection = (__endZ > __startZ) ? 1 : -1;
        float __sumPos = 0.0 , __nowPos = 0.0;
        float __DepthOfPos = fabsf(__endZ - __startZ);
        //
        activeComm.copy("");
        activeComm.catf("M3 S%.3f\n", (double)__spindleRPM);
        activeComm.catf("G0 X%.3f\n", (double)__startX);
        activeComm.catf("G0 Z%.3f\n", (double)__startZ);
        f->Write(activeComm.c_str());
        while(__DepthOfPos > __sumPos){
            activeComm.copy("");
            __sumPos += __DepthOfPos;
            if(__DepthOfPos >= __sumPos){                
                __nowPos = __startZ + __sumPos * iDirection;
            }
            else{
                __nowPos = __startZ + __DepthOfPos * iDirection;
            }
            //to Z start
            activeComm.catf("G1 Z%.3f F%.3f\n", (double)__nowPos, (double)__Feedrate);
            activeComm.cat("G4 P10\n");
            //back to Z start
            activeComm.catf("G0 Z%.3f\n", (double)__startZ);
            f->Write(activeComm.c_str());
        }
        
        //end leave
        activeComm.copy("");
        activeComm.catf("G0 Z%.3f\n", (double)__startZ);
        activeComm.catf("G0 X%.3f\n", (double)__startX);
        activeComm.catf("M5 S\n");
        f->Write(activeComm.c_str());
        if (!f->Close())
        {
            reply.printf("M2009 file not closed");
            HandleReply(gb, GCodeResult::error, reply.c_str());
            return true;
        }
        DoFileMacro(gb, _filename, true, code);
        return true;
    }
}
// End


