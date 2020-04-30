/**
Creo plugin: makes screenshot after model/assy regeneration for animation purpose.
 */

#include <ProCore.h>
#include <ProMenuBar.h>
#include <ProMessage.h>
#include <ProUICmd.h>
#include <ProUIDialog.h>
#include <ProUIMessage.h>
#include <ProUIPushbutton.h>
#include <ProUITextarea.h>
#include <ProUtil.h>
#include <ProWindows.h>
#include <ProWstring.h>
#include <ProWTUtils.h>
#include <ProPDF.h>
#include <vector>

using namespace std;

static uiCmdAccessState AccessAvailable(uiCmdAccessMode);
void makeDialogWindow();
void initializeMsgFile();

void cancelAction(char*, char*, ProAppData);
void screenshotAction(char*, char*, ProAppData);

ProFileName msgFile;
char dialogName[] = { "Animate" };
char cancel[] = { "Cancel" };
char screenshot[] = { "Zrzut ekranu" };


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
	ProUIDialogPushbuttonAdd(dialogName, cancel, &gridOpts);
	ProStringToWstring(label[0], cancel);
	ProUIPushbuttonTextSet(dialogName, cancel, label[0]);
	ProUIPushbuttonActivateActionSet(dialogName, cancel, cancelAction, NULL);

	gridOpts.row = 2;
	ProUIDialogPushbuttonAdd(dialogName, screenshot, &gridOpts);
	ProStringToWstring(label[0], screenshot);
	ProUIPushbuttonTextSet(dialogName, screenshot, label[0]);
	ProUIPushbuttonActivateActionSet(dialogName, screenshot, screenshotAction, NULL);

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

void screenshotAction(char* dialog, char* component, ProAppData data)
{
	int window_id;
	ProPath output_file;

	ProWindowCurrentGet(&window_id);
	ProDirectoryCurrentGet(output_file);
	ProWstringConcatenate((wchar_t*)L"image.jpg", output_file, 9);
	ProRasterFileWrite(window_id, PRORASTERDEPTH_24, 5, 4, PRORASTERDPI_300, PRORASTERTYPE_JPEG, output_file);
}
