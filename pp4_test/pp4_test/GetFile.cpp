#include "GetFile.h"



GetFile::GetFile()
{
}


GetFile::~GetFile()
{
}

void GetFile::SetTextureName(wchar_t TexName)
{
	TexName = m_TexName;
}
wchar_t GetFile::GetSetTextureName()
{
	return m_TexName;
}
void GetFile::SetFileName(char* filename)
{
	filename = m_FileName;
}
char* GetFile::GetFileName()
{
	return m_FileName;
}
