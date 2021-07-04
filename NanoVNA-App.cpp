
#include <vcl.h>

#pragma hdrstop

#include <tchar.h>

#include <Vcl.Styles.hpp>
#include <Vcl.Themes.hpp>

USEFORM("SettingsUnit.cpp", SettingsForm);
USEFORM("ScreenCapture.cpp", VNAScreenCaptureForm);
USEFORM("UploadFirmwareUnit.cpp", UploadFirmwareForm);
USEFORM("UploadFirmwareV2Unit.cpp", UploadFirmwareV2Form);
USEFORM("VNAUsartUnit.cpp", VNAUsartCommsForm);
USEFORM("Unit1.cpp", Form1);
USEFORM("CommsUnit.cpp", CommsForm);
USEFORM("BatteryVoltageUnit.cpp", BatteryVoltageForm);
USEFORM("CalibrationUnit.cpp", CalibrationForm);
USEFORM("GraphUnit.cpp", GraphForm);
USEFORM("ColourPickerUnit.cpp", ColourPickerForm);
//---------------------------------------------------------------------------
int WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int)
{
	try
	{
		Application->Initialize();
		Application->HintPause = 50;
		Application->HintHidePause = 5000;
		Application->MainFormOnTaskBar = true;
		TStyleManager::TrySetStyle("Emerald Light Slate");
		Application->Title = "NanoVNA-App";
		Application->CreateForm(__classid(TForm1), &Form1);
		Application->CreateForm(__classid(TCommsForm), &CommsForm);
		Application->CreateForm(__classid(TBatteryVoltageForm), &BatteryVoltageForm);
		Application->CreateForm(__classid(TSettingsForm), &SettingsForm);
		Application->CreateForm(__classid(TCalibrationForm), &CalibrationForm);
		Application->CreateForm(__classid(TVNAScreenCaptureForm), &VNAScreenCaptureForm);
		Application->CreateForm(__classid(TVNAUsartCommsForm), &VNAUsartCommsForm);
		Application->CreateForm(__classid(TUploadFirmwareForm), &UploadFirmwareForm);
		Application->CreateForm(__classid(TUploadFirmwareV2Form), &UploadFirmwareV2Form);
		Application->CreateForm(__classid(TColourPickerForm), &ColourPickerForm);
		Application->Run();
	}
	catch (Exception &exception)
	{
		Application->ShowException(&exception);
	}
	catch (...)
	{
		try
		{
			throw Exception("");
		}
		catch (Exception &exception)
		{
			Application->ShowException(&exception);
		}
	}
	return 0;
}

