#include "logger.h"
Log::Log(char* filename)
{
	m_stream.clear();
	m_stream.open(filename);
}

void Log::Write(char* logline)
{
	m_stream<<logline<<endl;

}

Log::~Log()
{
	m_stream.close();
}