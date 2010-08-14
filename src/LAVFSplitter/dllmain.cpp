/*
 *      Copyright (C) 2010 Hendrik Leppkes
 *      http://www.1f0.de
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *  http://www.gnu.org/copyleft/gpl.html
 */

// Based on the SampleParser Template by GDCL
// --------------------------------------------------------------------------------
// Copyright (c) GDCL 2004. All Rights Reserved. 
// You are free to re-use this as the basis for your own filter development,
// provided you retain this copyright notice in the source.
// http://www.gdcl.co.uk
// --------------------------------------------------------------------------------

#include "stdafx.h"
#include "LAVFSplitter.h"

// Initialize the GUIDs
#include <InitGuid.h>
#include "moreuuids.h"

// The GUID we use to register the splitter media types
DEFINE_GUID(MEDIATYPE_LAVFSplitter,
  0x9c53931c, 0x7d5a, 0x4a75, 0xb2, 0x6f, 0x4e, 0x51, 0x65, 0x4d, 0xb2, 0xc0);

// --- COM factory table and registration code --------------

const AMOVIESETUP_MEDIATYPE 
  sudMediaTypes[] = 
{
  {
    &MEDIATYPE_Video,
      &MEDIASUBTYPE_NULL
  },
  {
    &MEDIATYPE_Audio,
      &MEDIASUBTYPE_NULL
    }
};

const AMOVIESETUP_PIN sudOutputPins[] = 
{
  {
    L"Video Output",      // pin name
      FALSE,              // is rendered?    
      TRUE,               // is output?
      FALSE,              // zero instances allowed?
      FALSE,              // many instances allowed?
      &CLSID_NULL,        // connects to filter (for bridge pins)
      NULL,               // connects to pin (for bridge pins)
      1,                  // count of registered media types
      &sudMediaTypes[0]       // list of registered media types    
  },
  {
    L"Audio Output",      // pin name
      FALSE,              // is rendered?    
      TRUE,               // is output?
      FALSE,              // zero instances allowed?
      FALSE,              // many instances allowed?
      &CLSID_NULL,        // connects to filter (for bridge pins)
      NULL,               // connects to pin (for bridge pins)
      1,                  // count of registered media types
      &sudMediaTypes[1]   // list of registered media types    
  },
};

const AMOVIESETUP_FILTER sudFilterReg =
{
  &__uuidof(CLAVFSplitter),       // filter clsid
  L"lavf dshow source filter",  // filter name
  MERIT_NORMAL,                   // merit
  2,                              // count of registered pins
  sudOutputPins                   // list of pins to register
};

// --- COM factory table and registration code --------------

// DirectShow base class COM factory requires this table, 
// declaring all the COM objects in this DLL
CFactoryTemplate g_Templates[] = {
  // one entry for each CoCreate-able object
  {
    sudFilterReg.strName,
      sudFilterReg.clsID,
      CLAVFSplitter::CreateInstance,
      NULL,
      &sudFilterReg
  },
};
int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]);

// self-registration entrypoint
STDAPI DllRegisterServer()
{
  std::list<LPCWSTR> chkbytes;

  // MKV/WEBM
  chkbytes.push_back(L"0,4,,1A45DFA3");

  // MPEG1
  chkbytes.push_back(L"0,16,FFFFFFFFF100010001800001FFFFFFFF,000001BA2100010001800001000001BB");
  // MPEG-PS
  chkbytes.push_back(L"0,5,FFFFFFFFC0,000001BA40");
  // MPEG-PVA
  chkbytes.push_back(L"0,8,fffffc00ffe00000,4156000055000000");
  // MPEG-TS
  chkbytes.push_back(L"0,1,,47,188,1,,47,376,1,,47");
  chkbytes.push_back(L"4,1,,47,196,1,,47,388,1,,47");
  chkbytes.push_back(L"0,4,,54467263,1660,1,,47");

  // AVI
  chkbytes.push_back(_T("0,4,,52494646,8,4,,41564920")); // 'RIFF' ... 'AVI '
  chkbytes.push_back(_T("0,4,,52494646,8,4,,41564958")); // 'RIFF' ... 'AVIX'

  // MP4
  chkbytes.push_back(_T("4,4,,66747970")); // ftyp
  chkbytes.push_back(_T("4,4,,6d6f6f76")); // moov
  chkbytes.push_back(_T("4,4,,6d646174")); // mdat
  chkbytes.push_back(_T("4,4,,736b6970")); // skip
  chkbytes.push_back(_T("4,12,ffffffff00000000ffffffff,77696465027fe3706d646174")); // wide ? mdat
  chkbytes.push_back(_T("3,3,,000001")); // mpeg4 video

  // FLV
  chkbytes.push_back(_T("0,4,,464C5601"));

  RegisterSourceFilter(__uuidof(CLAVFSplitter),
    MEDIATYPE_LAVFSplitter,
    chkbytes,
    L".mkv", L".mka", L".mks", // MKV
    L".avi", L".divx",  // AVI
    L".ts", L".m2ts", L".mpg", // MPEG
    L".mp4", L".mov", // MP4
    L".flv", L".webm" // Web Formats
    );

  // base classes will handle registration using the factory template table
  return AMovieDllRegisterServer2(true);
}

STDAPI DllUnregisterServer()
{
  UnRegisterSourceFilter(MEDIATYPE_LAVFSplitter);

  // base classes will handle de-registration using the factory template table
  return AMovieDllRegisterServer2(false);
}

// if we declare the correct C runtime entrypoint and then forward it to the DShow base
// classes we will be sure that both the C/C++ runtimes and the base classes are initialized
// correctly
extern "C" BOOL WINAPI DllEntryPoint(HINSTANCE, ULONG, LPVOID);
BOOL WINAPI DllMain(HANDLE hDllHandle, DWORD dwReason, LPVOID lpReserved)
{
  return DllEntryPoint(reinterpret_cast<HINSTANCE>(hDllHandle), dwReason, lpReserved);
}
