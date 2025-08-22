#include "cJSON.h"
#include "chatgpt.h"
#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static size_t	write_callback(void *contents, size_t size, size_t nmemb,
		void *userp)
{
	size_t			total_size;
	struct memory	*mem;
	char			*ptr;

	total_size = size * nmemb;
	mem = (struct memory *)userp;
	ptr = realloc(mem->response, mem->size + total_size + 1);
	if (!ptr)
		return (0);
	mem->response = ptr;
	memcpy(&(mem->response[mem->size]), contents, total_size);
	mem->size += total_size;
	mem->response[mem->size] = 0;
	return (total_size);
}

char	*create_json(const char *prompt)
{
	cJSON	*root;
	cJSON	*messages;
	cJSON	*msg;
	char	*json_data;

	root = cJSON_CreateObject();
	cJSON_AddStringToObject(root, "model", "gpt-5");
	messages = cJSON_CreateArray();
	msg = cJSON_CreateObject();
	cJSON_AddStringToObject(msg, "role", "user");
	cJSON_AddStringToObject(msg, "content", prompt);
	cJSON_AddItemToArray(messages, msg);
	cJSON_AddItemToObject(root, "messages", messages);
	json_data = cJSON_PrintUnformatted(root);
	cJSON_Delete(root);
	return (json_data);
}

struct curl_slist	*create_headers(CURL *curl, char *json_data,
		const char *api_key, struct memory *chunk)
{
	struct curl_slist	*headers;
	char				auth_header[512];

	headers = NULL;
	snprintf(auth_header, sizeof(auth_header), "Authorization: Bearer %s",
		api_key);
	headers = curl_slist_append(headers, "Content-Type: application/json");
	headers = curl_slist_append(headers, auth_header);
	curl_easy_setopt(curl, CURLOPT_URL,
		"https://api.openai.com/v1/chat/completions");
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)chunk);
	return (headers);
}

static char	*parse_chatgpt_content(const char *json_text)
{
	char	*result;

	cJSON *resp_json, *error_obj, *error_message, *choices, *first_choice,
		*message, *content;
	result = NULL;
	resp_json = cJSON_Parse(json_text);
	error_obj = cJSON_GetObjectItem(resp_json, "error");
	if (error_obj)
	{
		error_message = cJSON_GetObjectItem(error_obj, "message");
		if (error_message && cJSON_IsString(error_message))
		{
			fprintf(stderr, "API Error: %s\n", error_message->valuestring);
		}
		else
		{
			fprintf(stderr, "API returned an error, but no message.\n");
		}
		return (NULL);
	}
	if (!resp_json)
		goto cleanup;
	choices = cJSON_GetObjectItem(resp_json, "choices");
	if (!cJSON_IsArray(choices) || cJSON_GetArraySize(choices) == 0)
		goto cleanup;
	first_choice = cJSON_GetArrayItem(choices, 0);
	message = cJSON_GetObjectItem(first_choice, "message");
	content = cJSON_GetObjectItem(message, "content");
	if (cJSON_IsString(content) && content->valuestring)
		result = strdup(content->valuestring);
cleanup:
	if (resp_json)
		cJSON_Delete(resp_json);
	return (result);
}

char	*chatgpt_query(const char *api_key, const char *prompt)
{
	char				*json_data;
	struct curl_slist	*headers;
	CURL				*curl;
	CURLcode			res;
	struct memory		chunk;
	char				*result;

	chunk = (struct memory){0};
	json_data = create_json(prompt);
	if (!json_data)
		return (NULL);
	curl = curl_easy_init();
	if (!curl)
	{
		free(json_data);
		return (NULL);
	}
	headers = create_headers(curl, json_data, api_key, &chunk);
	res = curl_easy_perform(curl);
	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);
	free(json_data);
	if (res != CURLE_OK)
	{
		free(chunk.response);
		return (NULL);
	}
	result = parse_chatgpt_content(chunk.response);
	free(chunk.response);
	return (result);
}
