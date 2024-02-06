

#pragma once
#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include <fstream>
#include <string>
#include <map>

enum	ResponseType { SMALL_RESPONSE, LARGE_RESPONSE };

class HttpResponse
{

	private:
		ResponseType						type;
		std::string							version;
		std::string							statusCode;
		std::string							statusMessage;
		std::map<std::string, std::string>	headers;
		std::string							body;

		std::string							filePath;
		size_t								fileSize;

	public:
		HttpResponse();

		

		void			setType(ResponseType typeValue);
		void			setVersion(const std::string& versionValue);
		void			setStatusCode(const std::string& statusCodeValue);
		void			setStatusMessage(const std::string& statusMessageValue);
		void			setHeader(const std::string& key, const std::string& value);
		void			setBody(const std::string& bodyValue);
		void			setFilePath(const std::string& filePathValue);
		void			setFileSize(size_t fileSizeValue);


		ResponseType	getType() const;
		std::string		getVersion() const;
		std::string		getStatusCode() const;
		std::string		getStatusMessage() const;
		std::string		getHeader(const std::string& key) const;
		std::string		getBody() const;
		std::string		getFilePath() const;
		size_t			getFileSize() const;


		std::string getStatusLine() const;
		std::string getHeadersAsString() const;

		std::string buildResponse() const; // for small files

		void	generateStandardErrorResponse(const std::string &statusCodeValue, const std::string &statusMessageValue, const std::string &title, const std::string &detail = "");

};

#endif /* HTTPRESPONSE_HPP */
