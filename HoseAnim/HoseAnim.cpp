/**
Creo plugin: makes screenshots in a loop after model/assy regeneration and parameter update.
 */

#include <ProCore.h>
#include <ProMenuBar.h>
#include <ProUIDialog.h>
#include <ProUIInputpanel.h>
#include <ProUILabel.h>
#include <ProUIMessage.h>
#include <ProUIPushbutton.h>
#include <ProUtil.h>
#include <ProWindows.h>
#include <ProWstring.h>
#include <ProParameter.h>
#include <ProSolid.h>
#include <vector>

using namespace std;

static uiCmdAccessState AccessAvailable(uiCmdAccessMode);
void makeDialogWindow();
void initializeMsgFile();

void cancelAction(char*, char*, ProAppData);
void startAction(char*, char*, ProAppData);

ProFileName msgFile;
char dialogName[] = { "Animate" };
char parameter_l[] = { "Parametr:" };
char parameter_v[] = { "parametr" };
char parameter_init_l[] = { "Wartosc poczatkowa:" };
char parameter_init_v[] = { "init" };
char parameter_end_l[] = { "Wartosc koncowa:" };
char parameter_end_v[] = { "end" };
char parameter_step_l[] = { "Krok:" };
char parameter_step_v[] = { "step" };
char cancel[] = { "Cancel" };
char start[] = { "Start" };
int window_id;


extern "C" int main(int argc, char** argv)
{
	ProToolkitMain(argc, argv);
	return 0;
}

extern "C" int user_initialize()
{
	ProErr err = PRO_TK_NO_ERROR;

	initializeMsgFile();

	uiCmdCmdId hose_anim_cmd_id;

	char actionName[] = "HoseAnimate";
	ProCmdActionAdd(actionName, (uiCmdCmdActFn)makeDialogWindow,
		uiCmdPrioDefault, AccessAvailable, PRO_B_FALSE,
		PRO_B_FALSE, &hose_anim_cmd_id);
	char parentMenu[] = "Utilities";
	char buttonName[] = "Animate";
	char buttonLabel[] = "Animate";
	ProMenubarmenuPushbuttonAdd(parentMenu, buttonName, buttonName, buttonLabel, NULL,
		PRO_B_TRUE, hose_anim_cmd_id, msgFile);

	return 0;
}

extern "C" void user_terminate()
{
	return;
}

void makeDialogWindow()
{
	ProWindowCurrentGet(&window_id);
	int exit_status;
	ProUIDialogCreate(dialogName, NULL);
	ProUIGridopts gridOpts;
	PRO_UI_GRIDOPTS_DEFAULT(gridOpts);

	ProWstring* label;
	ProArrayAlloc(1, sizeof(wchar_t), 1, (ProArray*)&label);
	label[0] = (wchar_t*)calloc(PRO_COMMENT_SIZE, sizeof(wchar_t));


	gridOpts.horz_cells = 1;
	gridOpts.vert_cells = 1;
	gridOpts.row = 1;
	gridOpts.column = 1;
	gridOpts.horz_resize = PRO_B_FALSE;
	gridOpts.vert_resize = PRO_B_FALSE;


	gridOpts.row = 1;
	gridOpts.column = 1;
	ProUIDialogLabelAdd(dialogName, parameter_l, &gridOpts);
	ProStringToWstring(label[0], parameter_l);
	ProUILabelTextSet(dialogName, parameter_l, label[0]);
	gridOpts.column = 2;
	ProUIDialogInputpanelAdd(dialogName, parameter_v, &gridOpts);


	gridOpts.row = 2;
	gridOpts.column = 1;
	ProUIDialogLabelAdd(dialogName, parameter_init_l, &gridOpts);
	ProStringToWstring(label[0], parameter_init_l);
	ProUILabelTextSet(dialogName, parameter_init_l, label[0]);
	gridOpts.column = 2;
	ProUIDialogInputpanelAdd(dialogName, parameter_init_v, &gridOpts);

	gridOpts.row = 3;
	gridOpts.column = 1;
	ProUIDialogLabelAdd(dialogName, parameter_end_l, &gridOpts);
	ProStringToWstring(label[0], parameter_end_l);
	ProUILabelTextSet(dialogName, parameter_end_l, label[0]);
	gridOpts.column = 2;
	ProUIDialogInputpanelAdd(dialogName, parameter_end_v, &gridOpts);

	gridOpts.row = 4;
	gridOpts.column = 1;
	ProUIDialogLabelAdd(dialogName, parameter_step_l, &gridOpts);
	ProStringToWstring(label[0], parameter_step_l);
	ProUILabelTextSet(dialogName, parameter_step_l, label[0]);
	gridOpts.column = 2;
	ProUIDialogInputpanelAdd(dialogName, parameter_step_v, &gridOpts);

	gridOpts.row = 5;
	gridOpts.column = 1;
	ProUIDialogPushbuttonAdd(dialogName, cancel, &gridOpts);
	ProStringToWstring(label[0], cancel);
	ProUIPushbuttonTextSet(dialogName, cancel, label[0]);
	ProUIPushbuttonActivateActionSet(dialogName, cancel, cancelAction, NULL);
	gridOpts.column = 2;
	ProUIDialogPushbuttonAdd(dialogName, start, &gridOpts);
	ProStringToWstring(label[0], start);
	ProUIPushbuttonTextSet(dialogName, start, label[0]);
	ProUIPushbuttonActivateActionSet(dialogName, start, startAction, NULL);

	ProUIDialogActivate(dialogName, &exit_status);
	ProUIDialogDestroy(dialogName);
}

static uiCmdAccessState AccessAvailable(uiCmdAccessMode access_mode)
{
	return (ACCESS_AVAILABLE);
}

void initializeMsgFile()
{
	char MSGFIL[] = "hoseanim.txt";
	ProStringToWstring(msgFile, MSGFIL);
}

void cancelAction(char* dialog, char* component, ProAppData data)
{
	ProUIDialogExit(dialog, 0);
}

void startAction(char* dialog, char* component, ProAppData data)
{
	wchar_t* paramName;
	ProUIInputpanelValueGet(dialogName, parameter_v, &paramName);

	ProParameter param;
	ProMdl model;
	ProModelitem modelItem;
	ProMdlCurrentGet(&model);
	ProMdlToModelitem(model, &modelItem);
	ProError err = ProParameterInit(&modelItem, paramName, &param);
	if (err == PRO_TK_NO_ERROR)
	{
		double initialValue, endValue, stepValue;
		wchar_t* iValue, *eValue, *sValue;
		ProUIInputpanelValueGet(dialogName, parameter_init_v, &iValue);
		initialValue = wcstod(iValue, NULL);
		ProUIInputpanelValueGet(dialogName, parameter_end_v, &eValue);
		endValue = wcstod(eValue, NULL);
		ProUIInputpanelValueGet(dialogName, parameter_step_v, &sValue);
		stepValue = wcstod(sValue, NULL);

		ProPath output_file;
		ProParamvalue value;
		value.type = PRO_PARAM_DOUBLE;
		value.value.d_val = initialValue;
		int counter = 0;
		wchar_t suffix[5];

		while (true) {
			ProParameterValueSet(&param, &value);

			ProSolidRegenerate((ProSolid)model, PRO_REGEN_NO_FLAGS);
			ProWindowRepaint(window_id);

			ProDirectoryCurrentGet(output_file);
			ProWstringConcatenate((wchar_t*)L"image", output_file, 5);
			swprintf(suffix, L"%05d", counter);
			ProWstringConcatenate(suffix, output_file, 5);
			ProWstringConcatenate((wchar_t*)L".jpg", output_file, 4);
			ProRasterFileWrite(window_id, PRORASTERDEPTH_24, 5, 4, PRORASTERDPI_300, PRORASTERTYPE_JPEG, output_file);

			if (initialValue < endValue)
			{
				value.value.d_val += stepValue;
				if (value.value.d_val > endValue)
					break;
			}
			else
			{
				value.value.d_val -= stepValue;
				if (value.value.d_val < endValue)
					break;
			}
			counter++;
		}

	}
	else
	{
		ProUIMessageButton* buttons;
		ProUIMessageButton userChoice;
		ProArrayAlloc(1, sizeof(ProUIMessageButton),
			1, (ProArray*)&buttons);
		buttons[0] = PRO_UI_MESSAGE_OK;
		ProUIMessageDialogDisplay(PROUIMESSAGE_INFO, (wchar_t*)L"Blad", (wchar_t*)L"Nieprawidlowy parametr", buttons, PRO_UI_MESSAGE_ABORT, &userChoice);
	}
}