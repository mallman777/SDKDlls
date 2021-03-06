// SDKMFC2015.cpp : Defines the initialization routines for the DLL.
//
#pragma once
#include "stdafx.h"
#include "SDKMFC2015.h"
#include "libSDK.h"
#include "interpret.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: If this DLL is dynamically linked against the MFC DLLs,
//		any functions exported from this DLL which call into
//		MFC must have the AFX_MANAGE_STATE macro added at the
//		very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

// CSDKMFC2015App

BEGIN_MESSAGE_MAP(CSDKMFC2015App, CWinApp)
END_MESSAGE_MAP()


// CSDKMFC2015App construction

CSDKMFC2015App::CSDKMFC2015App()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CSDKMFC2015App object

CSDKMFC2015App theApp;

const GUID CDECL _tlid = { 0x189D6F4E, 0xCDD, 0x47B7, { 0xA2, 0xE2, 0x3B, 0x0, 0x15, 0x8D, 0x76, 0x8 } };
const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;


// CSDKMFC2015App initialization

BOOL CSDKMFC2015App::InitInstance()
{
	CWinApp::InitInstance();

	// Register all OLE server (factories) as running.  This enables the
	//  OLE libraries to create objects from other applications.
	COleObjectFactory::RegisterAll();

	return TRUE;
}

// DllGetClassObject - Returns class factory

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return AfxDllGetClassObject(rclsid, riid, ppv);
}


// DllCanUnloadNow - Allows COM to unload DLL

STDAPI DllCanUnloadNow(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return AfxDllCanUnloadNow();
}


// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!AfxOleRegisterTypeLib(AfxGetInstanceHandle(), _tlid))
		return SELFREG_E_TYPELIB;

	if (!COleObjectFactory::UpdateRegistryAll())
		return SELFREG_E_CLASS;

	return S_OK;
}


// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!AfxOleUnregisterTypeLib(_tlid, _wVerMajor, _wVerMinor))
		return SELFREG_E_TYPELIB;

	if (!COleObjectFactory::UpdateRegistryAll(FALSE))
		return SELFREG_E_CLASS;

	return S_OK;
}

//CSpatialAnalyzerSDK baseNrkSdk;
libSDK NrkSdk;
interpret interp;

SDKLIBRARY_API long connToSA() {
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!NrkSdk.CreateDispatch(_T("SpatialAnalyzerSDK.Application")))
	{
		AfxMessageBox(_T("Something Failed in OnCreate."));
		return -1;  // connection failed.
	}
	long statCode;
	CString host = _T("localhost");
	NrkSdk.ConnectEx(host, &statCode);
	if (statCode){
		CString msg;
		msg.Format(_T("Connection Error, statCode: %d"), statCode);
		AfxMessageBox(msg);
	}
	return statCode;
}

//File Operations
//MP Task Overview
//MP Subroutines
//View Control
//Construction Operations
SDKLIBRARY_API void copyObject(char * sColl, char * sName, char * destCol, char * destName, bool overwrite) {
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CString sC = interp.convertCharStartToCString(sColl);
	CString sN = interp.convertCharStartToCString(sName);
	CString dC = interp.convertCharStartToCString(destCol);
	CString dN = interp.convertCharStartToCString(destName);
	NrkSdk.copyObject(sC, sN, dC, dN, overwrite);
}
SDKLIBRARY_API  void renamePoint(char* c1, char* o1, char* t1, char* c2, char* o2, char* t2, bool overwrite) {
	  AFX_MANAGE_STATE(AfxGetStaticModuleState());
	  CString C1 = interp.convertCharStartToCString(c1);
	  CString O1 = interp.convertCharStartToCString(o1);
	  CString T1 = interp.convertCharStartToCString(t1);
	  CString C2 = interp.convertCharStartToCString(c2);
	  CString O2 = interp.convertCharStartToCString(o2);
	  CString T2 = interp.convertCharStartToCString(t2);
	  NrkSdk.renamePoint(C1, O1, T1, C2, O2, T2, overwrite);
  }
  //Collections
SDKLIBRARY_API  void setOrConstructDefaultCollection(char* colName) {
	  AFX_MANAGE_STATE(AfxGetStaticModuleState());
	  CString cN = interp.convertCharStartToCString(colName);
	  NrkSdk.setOrConstructDefaultCollection(cN);
  }
SDKLIBRARY_API  void deleteCollection(char * colName) {
	  AFX_MANAGE_STATE(AfxGetStaticModuleState());
	  CString cN = interp.convertCharStartToCString(colName);
	  NrkSdk.deleteCollection(cN);
  }
SDKLIBRARY_API  void getActiveCollectionName(char * buf, size_t sz) {
	  AFX_MANAGE_STATE(AfxGetStaticModuleState());
	  NrkSdk.getActiveCollectionName(buf, sz);
  }
  //Points and Groups
SDKLIBRARY_API  void constructPointFitToPoints(char * buf, char * ptCol, char * ptObj, char * ptTarg) {
	  AFX_MANAGE_STATE(AfxGetStaticModuleState());
	  CStringArray ptNameList;
	  CString pC = interp.convertCharStartToCString(ptCol);
	  CString pO = interp.convertCharStartToCString(ptObj);
	  CString pT = interp.convertCharStartToCString(ptTarg);
	  interp.copyBufferToCStringArray(ptNameList, buf);
	  NrkSdk.constructPointFitToPoints(ptNameList, pC, pO, pT);
  }
SDKLIBRARY_API  void constructAPointInWorkingCoordinates(char * ptCol, char * ptObj, char * ptTarg, double x, double y, double z) {
	  AFX_MANAGE_STATE(AfxGetStaticModuleState());
	  CString pC = interp.convertCharStartToCString(ptCol);
	  CString pO = interp.convertCharStartToCString(ptObj);
	  CString pT = interp.convertCharStartToCString(ptTarg);
	  NrkSdk.constructAPointInWorkingCoordinates(pC, pO, pT, x, y, z);
	}
SDKLIBRARY_API  void constructAPointAtLineMidPoint(char * lCol, char * lName, char * ptCol, char * ptObj, char * ptTarg) {
	  AFX_MANAGE_STATE(AfxGetStaticModuleState());
	  CString lC = interp.convertCharStartToCString(lCol);
	  CString lN = interp.convertCharStartToCString(lName);
	  CString pC = interp.convertCharStartToCString(ptCol);
	  CString pO = interp.convertCharStartToCString(ptObj);
	  CString pT = interp.convertCharStartToCString(ptTarg);
	  NrkSdk.constructAPointAtLineMidPoint(lC, lN, pC, pO, pT);
	}
SDKLIBRARY_API  void constructAPointGroupFromPointNameRefList(char * buf, char * ptCol, char * ptObj) {
	  AFX_MANAGE_STATE(AfxGetStaticModuleState());
	  CStringArray ptNameList;
	  CString pC = interp.convertCharStartToCString(ptCol);
	  CString pO = interp.convertCharStartToCString(ptObj);
	  interp.copyBufferToCStringArray(ptNameList, buf);
	  NrkSdk.constructAPointGroupFromPointNameRefList(ptNameList, pC, pO);
	}
SDKLIBRARY_API  void constructAPointAtCircleCenter(char * cirCol, char * cirName, char * ptCol, char * ptObj, char * ptTarg) {
	  AFX_MANAGE_STATE(AfxGetStaticModuleState());
	  CString cC = interp.convertCharStartToCString(cirCol);
	  CString cN = interp.convertCharStartToCString(cirName);
	  CString pC = interp.convertCharStartToCString(ptCol);
	  CString pO = interp.convertCharStartToCString(ptObj);
	  CString pT = interp.convertCharStartToCString(ptTarg);
	  NrkSdk.constructAPointAtCircleCenter(cC, cN, pC, pO, pT);
	}
  //Lines
SDKLIBRARY_API  void constructLine2Points(char* lCol, char* lName, char* fPtCol, char* fPtObj, char* fPtTarg, char* sPtCol, char* sPtObj, char* sPtTarg) {
	  AFX_MANAGE_STATE(AfxGetStaticModuleState());
	  CString lC = interp.convertCharStartToCString(lCol);
	  CString lN = interp.convertCharStartToCString(lName);
	  CString fC = interp.convertCharStartToCString(fPtCol);
	  CString fO = interp.convertCharStartToCString(fPtObj);
	  CString fT = interp.convertCharStartToCString(fPtTarg);
	  CString sC = interp.convertCharStartToCString(sPtCol);
	  CString sO = interp.convertCharStartToCString(sPtObj);
	  CString sT = interp.convertCharStartToCString(sPtTarg);
	  NrkSdk.constructLine2Points(lC, lN, fC, fO, fT, sC, sO, sT);
	}
  //Frames
SDKLIBRARY_API  void constructFrame(char* fCol, char* fName, double* T) {
	  AFX_MANAGE_STATE(AfxGetStaticModuleState());
	  CString fC = interp.convertCharStartToCString(fCol);
	  CString fN = interp.convertCharStartToCString(fName);
	  NrkSdk.constructFrame(fC, fN, T);
  }
  //Other MP Types
SDKLIBRARY_API  void makePointNameRefListRuntimeSelect(char * buf, size_t sz, const char * msg) {
	  AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CString outMsg = interp.convertCharStartToCString(msg);
	NrkSdk.getPointNameRefListRunTimeSelect(buf, sz, outMsg);
}
SDKLIBRARY_API  void getWorkingTransformOfObjectFixedXYZ(char * col, char * name, double * T) {
	  AFX_MANAGE_STATE(AfxGetStaticModuleState());
	  CString c = interp.convertCharStartToCString(col);
	  CString n = interp.convertCharStartToCString(name);
	  NrkSdk.getWorkingTransformOfObjectFixedXYZ(c, n, T);
  }
//Analysis Operations
SDKLIBRARY_API  void transformObjectsByDeltaAboutWorkingFrame(char * buf, double* T) {
	  AFX_MANAGE_STATE(AfxGetStaticModuleState());
		CStringArray objs;
		interp.copyBufferToCStringArray(objs, buf);
		NrkSdk.transformObjectsByDeltaAboutWorkingFrame(objs, T);
 }
//Reporting Operations
//Excel Direct Connect
//MS Office Reporting Operations
//Process Flow Operations
//Scalar Math Operations
//Vector Math Operations
//Instrument Operations
//Cloud Viewer Operations
//Variables
//Utility Operations
SDKLIBRARY_API	void setWorkingFrame(char* fCol, char* fName) {
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		CString fC = interp.convertCharStartToCString(fCol);
		CString fN = interp.convertCharStartToCString(fName);
		NrkSdk.setWorkingFrame(fC, fN);
	}
SDKLIBRARY_API	void deleteObjects(char * buf) {
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		CStringArray objs;
		interp.copyBufferToCStringArray(objs, buf);
		NrkSdk.deleteObjects(objs);
	}

SDKLIBRARY_API void displayMsg(char* msg) {
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CString outMsg = interp.convertCharStartToCString(msg);
	AfxMessageBox(outMsg);
}
SDKLIBRARY_API void test(char * buf, int sz) {
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CString msg = _T("running Test");
	NrkSdk.getTest(buf, sz, msg);
}
