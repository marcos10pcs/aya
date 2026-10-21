#pragma once

#include <string>
#include "Utility/HttpAsync.hpp"

class SimpleJSON;

#ifdef RBX_TEST_BUILD
void SetDefaultFilePath(const std::string& path);
const std::string& GetDefaultFilePath();
#endif

void SetMasterBaseURL(const std::string& baseUrl);
const std::string& GetMasterBaseURL();

void SetBaseURL(const std::string& baseUrl);
const std::string& GetBaseURL();

void SetPlaceKey(const std::string& placeKey);
const std::string& GetPlaceKey();

void ReportStatisticWithMessage(const std::string& baseUrl, const std::string& id, const std::string& simpleMessage,
    const char* secondaryFilterName = NULL, const char* secondaryFilterValue = NULL);

void ReportStatistic(const std::string& baseUrl, const std::string& id, const std::string& primaryFilterName, const std::string& primaryFilterValue,
    const std::string& secondaryFilterName, const std::string& secondaryFilterValue);

void ReportStatisticPost(const std::string& baseUrl, const std::string& id, const std::string& postData, const char* secondaryFilterName,
    const char* secondaryFilterValue);


std::string UploadLogFile(const std::string& baseUrl, const std::string& data);

void SetFetchLocalClientSettings(bool status);
bool FetchLocalClientSettingsData(const char* group, SimpleJSON* dest);
void LoadClientSettingsFromString(const char* group, const std::string& settingsData, SimpleJSON* dest);
bool FetchClientSettingsData(const char* group, const char* apiKey, SimpleJSON* dest);
void FetchClientSettingsData(const char* group, const char* apiKey, std::string* dest);
Aya::HttpFuture FetchClientSettingsDataAsync(const char* group, const char* apiKey);
Aya::HttpFuture FetchABTestDataAsync(const std::string& url);
std::string LoadABTestFromString(const std::string& responseData);