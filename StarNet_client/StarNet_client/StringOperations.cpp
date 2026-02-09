#include "StringOperations.h"

// Разбивает строку string на подстроки по разделителю delimiter
char** SplitString(char* string, char* delimiter, int* numElements, bool ignoreEmpty)
{
	int nDelimiterLen = strlen(delimiter); // длина разделителя
	int nStringLen = strlen(string); // длина строки
	int nDelimMatch = 0;
	*numElements = 0; // выходной параметр (количество полученных подстрок)

	// Проверяет валидность разделителя
	if (nDelimiterLen == 0 || nDelimiterLen >= nStringLen)
		return (char**)NULL;

	// Выделяет память под массив подстрок 
	char* initial = string;
	char** buffer = (char**)malloc(((nStringLen / nDelimiterLen) + 1) * sizeof(char*));
	ZeroMemory(buffer, ((nStringLen / nDelimiterLen) + 1) * sizeof(char*));

	// Проходит по строке, ищет разделитель и сохраняет подстроки в buffer
	for (int i = 0; i < nStringLen; i++)
	{
		// поиск разделителя
		if (string[i] == delimiter[nDelimMatch])
			nDelimMatch++; // считает колво символов входящих в разделитель
		else {
			nDelimMatch = 0; // обнуляет
			if (string[i] == delimiter[nDelimMatch])
				nDelimMatch++;
		}

		if (nDelimMatch == nDelimiterLen) // совпадение подстроки с разделеителем
		{
			nDelimMatch = 0;

			char* data = initial;
			int dataLength = string + i - initial + 1 - nDelimiterLen;
			initial = string + i + 1;

			if (dataLength == 0 && ignoreEmpty)
				continue;

			buffer[*numElements] = (char*)malloc(dataLength + 1);
			if(dataLength) memcpy_s(buffer[*numElements], dataLength, data, dataLength);
			buffer[*numElements][dataLength] = '\0';
			*numElements = *numElements + 1;
		}
		
		if (i == nStringLen - 1) // запись в последнюю подстроку buffer
		{
			char* data = initial;
			int dataLength = string + i - initial + 1;

			if (dataLength == 0 && ignoreEmpty)
				continue;
				
			buffer[*numElements] = (char*)malloc(dataLength + 1);
			if (dataLength) memcpy_s(buffer[*numElements], dataLength, data, dataLength);
			buffer[*numElements][dataLength] = '\0';
			*numElements = *numElements + 1;
		}
	}
	return buffer;
}

// Освобождает память после SplitString
void FreeSplitStringBuffer(char** buffer, int numElements)
{
	for (int i = 0; i < numElements; i++) 
	{
		free(buffer[i]);
	}

	free(buffer);
}

// Объединяет массив строк в одну
char* JoinString(char** strings, int numElements, int* outputLength) {
	int size = 0;
	for (int i = 0; i < numElements; i++)
		size += strlen(strings[i]); // Вычисляет общий размер результирующей строки

	char* finalBuffer = (char*)malloc(size + 1); // Выделяет память под результат
	finalBuffer[size] = 0;

	for (int i = 0, index = 0; i < numElements; i++) { // Заполнение
		memcpy_s(finalBuffer + index, size - index, strings[i], strlen(strings[i]));
		index += strlen(strings[i]);
	}

	*outputLength = size; // выходной параметр (длина результирующей строки)
	return finalBuffer;
}

// Освобождает память после JoinString
void FreeJoinStringBuffer(char* buffer) {
	free(buffer);
}