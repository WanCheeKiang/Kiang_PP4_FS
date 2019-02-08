#pragma once
class GetFile
{
	wchar_t m_TexName;
	char* m_FileName;

public:
	GetFile();
	~GetFile();
	void SetTextureName(wchar_t TexName);
	wchar_t GetSetTextureName();
	void SetFileName(char* filename);
	char* GetFileName();
};

