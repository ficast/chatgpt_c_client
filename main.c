/**
 * @file main.c
 * @brief Example usage of the Simple ChatGPT C Client.
 *
 * This program demonstrates the basic usage of the chatgpt_query() function.
 * It initializes libcurl, retrieves an API key from the environment, sends a
 * hardcoded prompt to the OpenAI API, prints the response, and then cleans up.
 *
 * --- HOW TO COMPILE ---
 * gcc main.c chatgpt.c cJSON.c -o chat_app -lcurl
 *
 * --- HOW TO RUN ---
 * 1. Set your API key in an environment variable:
 *    export OPENAI_API_KEY="your_secret_key_here"
 * 2. Run the compiled application:
 *    ./chat_app
 *
 * @note Remember that the string returned by chatgpt_query() is dynamically
 *       allocated and must be freed by the caller using free().
 *
 * @license This project is licensed under the MIT License.
 */

#include "chatgpt.h"
#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>

int	main(void)
{
	const char	*api_key = getenv("OPENAI_API_KEY");
	const char	*prompt = "What's the capital of Brazil?";
	char		*response;

	if (!api_key)
	{
		fprintf(stderr,
			"Error: The environment variable OPENAI_API_KEY is not set.\n");
		fprintf(stderr, "Please set it before running this program.\n");
	}
	curl_global_init(CURL_GLOBAL_ALL);
	response = chatgpt_query(api_key, prompt);
	if (response)
	{
		printf("ChatGPT says: %s\n", response);
		free(response);
	}
	else
	{
		fprintf(stderr, "Failed to get response.\n");
	}
	curl_global_cleanup();
	return (0);
}
