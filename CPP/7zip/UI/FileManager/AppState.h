// AppState.h

#ifndef ZIP7_INC_APP_STATE_H
#define ZIP7_INC_APP_STATE_H

// #include <fstream>
// #include <chrono>
// #include <iomanip>
// #include <string>
// #include <sstream>
// #include <ctime>
// #include <Windows.h>

#include "../../../Common/Wildcard.h"

#include "../../../Windows/Synchronization.h"

#include "ViewSettings.h"

class CFastFolders
{
  NWindows::NSynchronization::CCriticalSection _criticalSection;
public:
  UStringVector Strings;
  void SetString(unsigned index, const UString &s)
  {
    NWindows::NSynchronization::CCriticalSectionLock lock(_criticalSection);
    while (Strings.Size() <= index)
      Strings.AddNew();
    Strings[index] = s;
  }
  UString GetString(unsigned index)
  {
    NWindows::NSynchronization::CCriticalSectionLock lock(_criticalSection);
    if (index >= Strings.Size())
      return UString();
    return Strings[index];
  }
  void Save()
  {
    NWindows::NSynchronization::CCriticalSectionLock lock(_criticalSection);
    SaveFastFolders(Strings);
  }
  void Read()
  {
    NWindows::NSynchronization::CCriticalSectionLock lock(_criticalSection);
    ReadFastFolders(Strings);
  }
};

class CFolderHistory
{
  NWindows::NSynchronization::CCriticalSection _criticalSection;
  UStringVector Strings;

  void Normalize();
  
public:
  
  void GetList(UStringVector &foldersHistory)
  {
    NWindows::NSynchronization::CCriticalSectionLock lock(_criticalSection);
    foldersHistory = Strings;
  }
  
  void AddString(const UString &s);

  void RemoveAll()
  {
    NWindows::NSynchronization::CCriticalSectionLock lock(_criticalSection);
    Strings.Clear();
  }
  
  void Save()
  {
    NWindows::NSynchronization::CCriticalSectionLock lock(_criticalSection);
    SaveFolderHistory(Strings);
  }
  
  void Read()
  {
    NWindows::NSynchronization::CCriticalSectionLock lock(_criticalSection);
    ReadFolderHistory(Strings);
    Normalize();
  }
};

class CPathState
{
public:
  UString selectFile;
  UString path;
  unsigned int index;
};

class CPathStack
{
  CObjectVector<CPathState> paths;
  unsigned int index;
  bool ignore;

public:
  
  void onOpenNewPath(const UString &path) {
    if (ignore) {
      return;
    }
    if (index < paths.Size() && CompareFileNames(paths[index].path, path) == 0)
      return;
    
    if (paths.IsEmpty()) {
      index = 0;
      // writeToFile(UString("input0: ") + path);
    } else {
      // writeToFile(UString("curr: ") + paths[index].path);
      // writeToFile(UString("input: ") + path);
      // UString temp = UString("index: ");
      // temp.Add_UInt32(index);
      // writeToFile(temp);
      index++;
    }

    if (index < paths.Size()) {
      paths.DeleteFrom(index);
    }

    CPathState state;
    state.path = path;
    state.index = index;
    paths.Add(state);
  }

  void onSelectChanged(const UString &fileName)
  {
    paths[index].selectFile = fileName;
  }

  // CPathState getBackwardPath()
  // {
  //   if (index <= 0)
  //     return CPathState();
  //   return paths[index - 1];
  // }

  // CPathState popBackwardPath()
  // {
  //   if (index <= 0)
  //     return CPathState();
  //   writeToFile(UString("popBackwardPath: ") + listToStr());
  //   // ignore = true;
  //   return paths[--index];
  // }
  
  // CPathState getForwardPath()
  // {
  //   if (index + 1 >= paths.Size())
  //     return CPathState();
  //   return paths[index + 1];
  // }
  
  // CPathState popForwardPath()
  // {
  //   if (index + 1 >= paths.Size())
  //     return CPathState();
  //   writeToFile(UString("popForwardPath: ") + listToStr());
  //   ignore = true;
  //   return paths[++index];
  // }
  
  CPathState getNavPath(bool forward)
  {
    if (forward) {
      if (index + 1 >= paths.Size())
        return CPathState();
      // writeToFile(UString("forward: ") + listToStr());
      ignore = true;
      return paths[++index];
    } else {
      if (index <= 0)
        return CPathState();
      // writeToFile(UString("backward: ") + listToStr());
      ignore = true;
      return paths[--index];
    }
  }

  void checkNavResult(bool forward, const UString &path) {
    ignore = false;
    if (index < 1) {
      return;
    }
    if (index >= paths.Size()) {
      return;
    }
    // UString log = UString("checkNavResult: index: ");
    // log.Add_UInt32(index);
    // log += ", path: ";
    // log += path;
    // writeToFile(log);
    if (path == paths[index].path) {
      return;
    }
    unsigned int lastIndex;
    if (forward) {
      lastIndex = index - 1;
    } else {
      lastIndex = index + 1;
    }
    if (path == paths[lastIndex].path) {
      index--;
      // log = UString("checkNavResult: update: ");
      // log.Add_UInt32(index);
      // writeToFile(log);
    } else {
      paths[index].path = path;
      // writeToFile(UString("checkNavResult: update: ") + path);
    }
  }
  
  // UString listToStr() {
  //   UString output;
  //   for (unsigned int i = 0; i < paths.Size(); i++)
  //   {
  //     CPathState state = paths[i];
  //     output.Add_UInt32(state.index);
  //     output += ".";
  //     output += state.path;
  //     output += " (";
  //     output += state.selectFile;
  //     output += ")";
  //     output.Add_LF();
  //   }
  //   output.Add_LF();
  //   output += "curr: ";
  //   output.Add_UInt32(index);
  //   output.Add_LF();
  //   output += "-----------------------";
  //   return output;
  // }

  // void writeToFile(const UString content) {
  //   // get current time
  //   auto now = std::chrono::system_clock::now();
  //   auto now_time_t = std::chrono::system_clock::to_time_t(now);
    
  //   std::tm localTime;
  //   localtime_s(&localTime, &now_time_t);

  //   // format time "YYYY-MM-DD HH:MM:SS"
  //   char timeStr[100];
  //   std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", &localTime);

  //   // get micro second
  //   auto now_us = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()) % 1000000;

  //   // add micro second to time
  //   std::string finalTimeStr = std::string(timeStr) + "." + std::to_string(now_us.count());

  //   // open file
  //   std::ofstream outFile("7z.log", std::ios::app);

  //   if (outFile.is_open()) {
  //       outFile.write(finalTimeStr.c_str(), finalTimeStr.size());
  //       outFile.put(' ');
  //       const std::wstring message = content.Ptr();
  //       // std::string narrowMessage(message.begin(), message.end());
  //       std::string narrowMessage = wstringToString(message);
  //       outFile.write(narrowMessage.c_str(), narrowMessage.size());
  //       outFile.put('\n');
  //       outFile.close();
  //       // std::cout << "Message written to file successfully." << std::endl;
  //   } else {
  //       // std::cerr << "Unable to open file for writing." << std::endl;
  //   }
  // }

  // std::string wstringToString(const std::wstring& wstr) {
  //   // get buffer size
  //   int bufferSize = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
  //   // allocate buffer
  //   std::string str(bufferSize, 0);
  //   WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], bufferSize, nullptr, nullptr);
  //   return str;
  // }
};


struct CAppState
{
  CFastFolders FastFolders;
  CFolderHistory FolderHistory;
  CPathStack PathStack;

  void Save()
  {
    FastFolders.Save();
    FolderHistory.Save();
  }
  void Read()
  {
    FastFolders.Read();
    FolderHistory.Read();
  }
};

#endif
