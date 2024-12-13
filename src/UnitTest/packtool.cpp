#include <Windows.h>
#include <io.h>

bool PackAllFiles(const string vFileName, const string vNameInPack, CPack* vpPack, bool vReplace) {
    _finddata_t FileData;
    intptr_t    hFile;

    string FileName   = TransformFileName(vFileName.c_str());
    string NameInPack = TransformFileName(vNameInPack.c_str());

    FileName == "." ? FileName = "" : FileName += "\\";
    NameInPack == "." ? NameInPack = "" : NameInPack += "\\";

    string FindName = FileName + "*.*";

    hFile = _findfirst(FindName.c_str(), &FileData);
    if (hFile == -1) {
        return false;
    } else {
        if (FileData.attrib & _A_SUBDIR) {

            string SubDir(FileData.name);
            if (SubDir != "." && SubDir != "..") {
                PackAllFiles(FileName + SubDir, NameInPack + SubDir, vpPack, vReplace);
            }
        }

        for (; _findnext(hFile, &FileData) == 0;) {
            if (FileData.attrib & _A_SUBDIR) {
                string SubDir(FileData.name);
                if (SubDir != "." && SubDir != "..") {
                    PackAllFiles(FileName + SubDir, NameInPack + SubDir, vpPack, vReplace);
                }
            } else {
                string SubName      = TransformFileName(FileData.name);
                string FileFullName = FileName + SubName;

                string Postfix =
                    FileFullName.substr(FileFullName.find_last_of('.') + 1);  //得到文件后缀
                if (mFilter.Filter(Postfix)) {                                //过滤
                    cout << FileFullName << "[被过滤]" << endl;
                } else {
                    cout << FileFullName << "[" << FileData.size << "] MD5[";

                    CCompress* pCompress = GetCompressByOpertion(FileFullName);
                    vpPack->AddFile(FileFullName, NameInPack + SubName, pCompress, vReplace);
                    cout << "]......" << endl;
                }
            }
        }
        _findclose(hFile);
    }
    return true;
}
