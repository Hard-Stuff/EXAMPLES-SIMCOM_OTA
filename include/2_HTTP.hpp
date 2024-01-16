#pragma once

// configs
#include <configs/networking.h>
#include <configs/certs/certificate.h>

// bricks
#include <1_SIMCOM.hpp>

// libs
#include <ArduinoHttpClient.h>

namespace HTTP
{
    HttpClient http(SIMCOM::client, SERVER, PORT);

#pragma region HTTP_STRUCTS
    struct Header
    {
        String key = "";
        String value = "";
    };
    const int MAX_HEADERS = 10;

    struct HttpRequest
    {
        Header headers[MAX_HEADERS]; // Response headers
        int header_count = 0;        // Number of headers in response
        String content = "";         // Request content

        /**
         * @brief Add a header to the request in a key: value fashion
         */
        void addHeader(const String &key, const String &value)
        {
            if (header_count < MAX_HEADERS)
            {
                headers[header_count].key = key;
                headers[header_count].value = value;
                header_count++;
            }
            else
                Serial.println("MAX HEADERS REACHED!");
        }

        /**
         * @brief Clear the contents of the HTTP request
         *
         * @param clear_all
         */
        void clear(bool ignore_headers = false)
        {
            content = ""; // Clear the content

            if (!ignore_headers)
            {
                // Reset headers and header count
                for (int i = 0; i < header_count; ++i)
                {
                    headers[i].key = "";
                    headers[i].value = "";
                }
                header_count = 0;
            }
        }

        /**
         * @brief Print the HttpRequest to serial (useful for debugging or analysing)
         *
         * @param output_stream Override the stream, e.g. if dumping to an FS file
         */
        void print(Stream *output_stream = &Serial)
        {
            output_stream->println("Headers:");
            for (int i = 0; i < header_count; i++)
            {
                output_stream->println(headers[i].key + " : " + headers[i].value);
            }
            output_stream->println("Content:");
            output_stream->println(content);
        }
    };

    struct HttpResponse
    {
        int status_code = 0;         // HTTP status code
        Header headers[MAX_HEADERS]; // Response headers
        int header_count = 0;        // Number of headers in response
        String body = "";            // Response body
        int content_length = 0;      // Content length
        bool is_chunked = false;     // Flag for chunked response
        String error_message = "";   // Error message, if any

        // Function to add a header (can be used while parsing the response)
        void addHeader(const String &key, const String &value)
        {
            if (header_count < MAX_HEADERS)
            {
                headers[header_count].key = key;
                headers[header_count].value = value;
                header_count = min(header_count + 1, MAX_HEADERS);
            }
            else
                Serial.println("MAX HEADERS REACHED!");
        }

        // Function to check if there was an error
        bool hasError() const { return !error_message.isEmpty(); }
        bool success() const { return status_code >= 200 && status_code < 300; }

        /**
         * @brief Print the HttpResponse to serial (useful for debugging or analysing)
         *
         * @param output_stream Override the stream, e.g. if dumping to an FS file
         */
        void print(Stream *output_stream = &Serial) const
        {
            Serial.println("Response status code: " + String(status_code));
            Serial.println(F("Response Headers:"));
            for (int i = 0; i <= header_count; i++)
                Serial.println("    " + headers[i].key + " : " + headers[i].value);
            Serial.println("Content length: " + String(content_length));
            Serial.println(F("Response:"));
            Serial.println(body);
            Serial.println("Response is " + String(is_chunked ? "" : "not ") + "chunked.");
            Serial.println("Body length is: " + String(body.length()));
        }

        /**
         * @brief Clear the contents of the HTTP response (should you wish to recycle the response element)
         */
        void clear()
        {
            status_code = 0;      // HTTP status code
            headers[MAX_HEADERS]; // Response headers
            header_count = 0;     // Number of headers in response
            body = "";            // Response body
            content_length = 0;   // Content length
            is_chunked = false;   // Flag for chunked response
            error_message = "";   // Error message, if any

            // Reset headers and header count
            for (int i = 0; i < header_count; ++i)
            {
                headers[i].key = "";
                headers[i].value = "";
            }
            header_count = 0;
        }
    };
#pragma endregion

    /**
     * @brief Any dedicated init processes
     *
     * @returns true if successful, otherwise false
     */
    bool init()
    {
        bool _s = true;
        SIMCOM::client.setTimeout(5000);
        return _s;
    }

    /**
     * @brief Post some content string to a given endpoint of the attached server
     *
     * @param endpoint Given endpoint, e.g. "/device_1/shadow"
     * @param request A request object that contains contents and headers
     * @return HttpResponse, the response as an HttpResponse object
     */
    HttpResponse postToHTTPServer(String endpoint, HttpRequest *request)
    {
        HttpResponse response;
        // Post the HTTP request
        http.beginRequest();
        http.post(endpoint);
        for (int i = 0; i < request->header_count; i++)
        {
            http.sendHeader(request->headers[i].key, request->headers[i].value);
        }
        http.sendHeader("Content-Length", request->content.length());
        http.beginBody();
        http.println(request->content);
        http.endRequest();

        if (SIMCOM::client.getWriteError() != 0)
        {
            response.status_code = HTTP_ERROR_TIMED_OUT;
            http.stop();
            return response;
        }

        Serial.print(F("Performing HTTP POST request... "));

        response.status_code = http.responseStatusCode();
        for (int i = 0; i < MAX_HEADERS; i++)
        {
            if (http.headerAvailable())
            {
                response.addHeader(http.readHeaderName(), http.readHeaderValue());
                response.header_count = i;
            }
            else
            {
                break;
            }
        }
        http.skipResponseHeaders();
        response.body = http.responseBody();
        return response;
    }

    /**
     * @brief Get the whatever contents from a given endpoint of the attached server
     *
     * @param endpoint Given endpoint, e.g. "/version"
     * @param request
     * @param skip_body Rarely used, only in circumstances where you want to stream the body response somewhere else (other than a string)
     * @return HttpResponse
     */
    HttpResponse getFromHTTPServer(String endpoint, HttpRequest *request = nullptr, bool skip_body = false)
    {
        HttpResponse response;
        // Get
        http.beginRequest();
        http.get(endpoint); 

        if (SIMCOM::client.getWriteError() != 0)
        {
            response.status_code = HTTP_ERROR_TIMED_OUT;
            http.stop();
            return response;
        }
        if (request != nullptr)
        {
            for (int i = 0; i < request->header_count; i++)
            {
                http.sendHeader(request->headers[i].key, request->headers[i].value);
            }
            if (request->content.length())
            {
                http.sendHeader("Content-Length", request->content.length());
                http.beginBody();
                http.println(request->content);
            }
        }
        http.endRequest();
        Serial.print(F("Performing HTTP GET request... "));

        response.status_code = http.responseStatusCode();
        for (int i = 0; i < MAX_HEADERS; i++)
        {
            if (http.headerAvailable())
            {
                response.addHeader(http.readHeaderName(), http.readHeaderValue());
                response.header_count = i;
            }
            else
            {
                break;
            }
        }
        response.content_length = http.contentLength();
        response.is_chunked = http.isResponseChunked();
        if (!skip_body)
        {
            response.body = http.responseBody();
        }
        return response;
    }
}