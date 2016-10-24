#pragma once

#include <exception>
#include <string>


namespace horst {

class Error : public std::exception {
public:
	Error(const std::string &msg);
	virtual ~Error() = default;

	const char *what() const noexcept override;

private:
	std::string message;
};

} // horst
