
# Simple ChatGPT C Client

This is a lightweight C client for interacting with the OpenAI Chat Completions API. The project provides a simple function to send a prompt to the API and receive the model's response (e.g., GPT-4, GPT-5).

## Features

-   **Simple Interface:** A single function, `chatgpt_query`, for easy integration.
-   **JSON Construction:** Dynamically creates the JSON payload required by the OpenAI API.
-   **HTTP Communication:** Uses `libcurl` for robust and reliable network communication.
-   **JSON Parsing:** Uses `cJSON` for efficient parsing of the API response.
-   **Error Handling:** Detects and reports errors returned by the API.

## Dependencies

To compile and use this project, you will need the following libraries:

1.  **libcurl:** A client-side URL transfer library.
    -   **On Debian/Ubuntu:** `sudo apt-get install libcurl4-openssl-dev`
    -   **On macOS (with Homebrew):** `brew install curl`

2.  **cJSON:** An ultralightweight JSON parser in C. The source files (`cJSON.c` and `cJSON.h`) are included within this project's structure.

## How to Compile

Assuming your source files are `chatgpt.c`, `cJSON.c` (available in this project's structure) and `main.c` (your main program), you can compile everything with the following command:

```bash
gcc main.c chatgpt.c cJSON.c -o chat_app -lcurl
```

-   `main.c`: The file containing your `main()` function.
-   `chatgpt.c`: The file containing the `chatgpt_query` function implementation.
-   `cJSON.c`: The implementation file for the cJSON library.
-   `-o chat_app`: Sets the name of the output executable.
-   `-lcurl`: Links with the `libcurl` library.

## How to Use

The main function provided is:

```c
char *chatgpt_query(const char *api_key, const char *prompt);
```

-   `api_key`: Your OpenAI API key.
-   `prompt`: The question or command you want to send to the model.
-   **Return Value:** The function returns a dynamically allocated string (`malloc`/`strdup`) containing the model's response. **It is the caller's responsibility to free this memory using `free()`**. In case of an error, the function returns `NULL`.

### Example `main.c`

```c
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

```

### Setting Up the API Key

To run the example above, set your API key in an environment variable:

```bash
export OPENAI_API_KEY="your_secret_key_here"
./chat_app
```

## Important Note on Design and Memory Management

The `chatgpt_query` function is designed to be **fully self-contained and encapsulated**. It initializes all necessary resources (including a `libcurl` handle) and releases them before returning.

-   **Advantage:** This makes it extremely easy to use and reuse. You don't need to worry about managing state or resources between calls.

-   **Disadvantage:** For applications that need to make a large number of calls in a short period (like a server or a long-running process), creating and destroying a `libcurl` handle for every call is inefficient and can lead to minor memory leaks in the underlying SSL layer.

This design choice also has implications for libcurl's global state. According to libcurl's official guidance, curl_global_init() should be called once when the program starts, and curl_global_cleanup() should be called once before it exits. Because this self-contained function does not follow that pattern, it is best suited for simple command-line tools rather than long-running applications or libraries.

For more intensive use cases, a more robust approach would be to modify the library to use a "session" or "context" pattern. In that pattern, a libcurl handle is created once, reused for multiple calls, and destroyed at the end, while the global functions are called only in main().

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.