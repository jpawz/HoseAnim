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
char measure1[] = { "Pomiar 1:" };
char measure1Value[] = { "measure1" };
char measure2[] = { "Pomiar 2:" };
char measure2Value[] = { "measure2" };
char measure3[] = { "Pomiar 3:" };
char measure3Value[] = { "measure3" };
char measure4[] = { "Pomiar 4:" };
char measure4Value[] = { "measure4" };
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
	ProUIDialogLabelAdd(dialogName, measure1, &gridOpts);
	ProStringToWstring(label[0], measure1);
	ProUILabelTextSet(dialogName, measure1, label[0]);
	gridOpts.column = 2;
	ProUIDialogInputpanelAdd(dialogName, measure1Value, &gridOpts);

	gridOpts.row = 6;
	gridOpts.column = 1;
	ProUIDialogLabelAdd(dialogName, measure2, &gridOpts);
	ProStringToWstring(label[0], measure2);
	ProUILabelTextSet(dialogName, measure2, label[0]);
	gridOpts.column = 2;
	ProUIDialogInputpanelAdd(dialogName, measure2Value, &gridOpts);

	gridOpts.row = 7;
	gridOpts.column = 1;
	ProUIDialogLabelAdd(dialogName, measure3, &gridOpts);
	ProStringToWstring(label[0], measure3);
	ProUILabelTextSet(dialogName, measure3, label[0]);
	gridOpts.column = 2;
	ProUIDialogInputpanelAdd(dialogName, measure3Value, &gridOpts);

	gridOpts.row = 8;
	gridOpts.column = 1;
	ProUIDialogLabelAdd(dialogName, measure4, &gridOpts);
	ProStringToWstring(label[0], measure4);
	ProUILabelTextSet(dialogName, measure4, label[0]);
	gridOpts.column = 2;
	ProUIDialogInputpanelAdd(dialogName, measure4Value, &gridOpts);

	gridOpts.row = 9;
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

	wchar_t* measure1Name;
	ProUIInputpanelValueGet(dialogName, measure1Value, &measure1Name);

	wchar_t* measure2Name;
	ProUIInputpanelValueGet(dialogName, measure2Value, &measure2Name);

	wchar_t* measure3Name;
	ProUIInputpanelValueGet(dialogName, measure3Value, &measure3Name);

	wchar_t* measure4Name;
	ProUIInputpanelValueGet(dialogName, measure4Value, &measure4Name);

	ProParameter param, param1, param2, param3, param4;
	ProMdl model;
	ProModelitem modelItem;
	ProMdlCurrentGet(&model);
	ProMdlToModelitem(model, &modelItem);
	ProError err = ProParameterInit(&modelItem, paramName, &param);

	ProError err1 = ProParameterInit(&modelItem, measure1Name, &param1);
	ProError err2 = ProParameterInit(&modelItem, measure2Name, &param2);
	ProError err3 = ProParameterInit(&modelItem, measure3Name, &param3);
	ProError err4 = ProParameterInit(&modelItem, measure4Name, &param4);

	ProUIMessageButton* buttons;
	ProUIMessageButton userChoice;
	ProArrayAlloc(1, sizeof(ProUIMessageButton),
		1, (ProArray*)&buttons);
	buttons[0] = PRO_UI_MESSAGE_OK;

	if (err == PRO_TK_NO_ERROR)
	{
		FILE* log;
		log = fopen("log.csv", "w");
		fprintf(log, "Param\tm1\tm2\tm3\tm4\n");
		double initialValue, endValue, stepValue;
		wchar_t* iValue, *eValue, *sValue;
		ProUIInputpanelValueGet(dialogName, parameter_init_v, &iValue);
		initialValue = wcstod(iValue, NULL);
		ProUIInputpanelValueGet(dialogName, parameter_end_v, &eValue);
		endValue = wcstod(eValue, NULL);
		ProUIInputpanelValueGet(dialogName, parameter_step_v, &sValue);
		stepValue = wcstod(sValue, NULL);

		if (stepValue <= 0)
		{
			ProUIMessageDialogDisplay(PROUIMESSAGE_INFO, (wchar_t*)L"Blad", (wchar_t*)L"Wartosc kroku musi byc wieksza od 0", buttons, PRO_UI_MESSAGE_ABORT, &userChoice);
			return;
		}

		ProPath output_file;
		ProParamvalue value;
		value.type = PRO_PARAM_DOUBLE;
		value.value.d_val = initialValue;
		ProParamvalue val1;
		val1.type = PRO_PARAM_DOUBLE;
		val1.value.d_val = -1;
		ProParamvalue val2;
		val2.type = PRO_PARAM_DOUBLE;
		val2.value.d_val = -1;
		ProParamvalue val3;
		val3.type = PRO_PARAM_DOUBLE;
		val3.value.d_val = -1;
		ProParamvalue val4;
		val4.type = PRO_PARAM_DOUBLE;
		val4.value.d_val = -1;
		int counter = 0;
		wchar_t suffix[5];

		while (true) {
			ProParameterValueSet(&param, &value);

			if (err1 == PRO_TK_NO_ERROR)
			{
				ProParameterValueGet(&param1, &val1);
			}
			if (err2 == PRO_TK_NO_ERROR)
			{
				ProParameterValueGet(&param2, &val2);
			}
			if (err3 == PRO_TK_NO_ERROR)
			{
				ProParameterValueGet(&param3, &val3);
			}
			if (err4 == PRO_TK_NO_ERROR)
			{
				ProParameterValueGet(&param4, &val4);
			}

			ProSolidRegenerate((ProSolid)model, PRO_REGEN_NO_FLAGS);
			ProSolidRegenerate((ProSolid)model, PRO_REGEN_NO_FLAGS);
			ProWindowRepaint(window_id);
			fprintf(log, "%f\t%f\t%f\t%f\t%f\n", value.value.d_val, val1.value.d_val, val2.value.d_val, val3.value.d_val, val4.value.d_val);
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
		ProUIMessageDialogDisplay(PROUIMESSAGE_INFO, (wchar_t*)L"Koniec", (wchar_t*)L"Koniec", buttons, PRO_UI_MESSAGE_ABORT, &userChoice);
		fclose(log);
	}
	else
	{
		ProUIMessageDialogDisplay(PROUIMESSAGE_INFO, (wchar_t*)L"Blad", (wchar_t*)L"Nieprawidlowy parametr", buttons, PRO_UI_MESSAGE_ABORT, &userChoice);
	}
}