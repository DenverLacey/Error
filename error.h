#pragma once

#include <iostream>
#include <variant>
#include <stdarg.h>
#include <sstream>

#define here_error(...) here_error_impl(__FILE__, __LINE__, __VA_ARGS__)

[[noreturn]]
void here_error_impl(const char *file, size_t line, const char *errfmt, va_list args) {
	fprintf(stderr, "Error @ %s:%zu: ", file, line);
	vfprintf(stderr, errfmt, args);
	fprintf(stderr, "\n");

	va_end(args);
	exit(EXIT_FAILURE);
}

[[noreturn]]
void here_error_impl(const char *file, size_t line, const char *errfmt, ...) {
	va_list args;
	va_start(args, errfmt);

	here_error_impl(file, line, errfmt, args);
}

#define here_verify(condition, ...) if (!(condition)) here_error(__VA_ARGS__)

#define todo(...) todo_impl(__FILE__, __LINE__, __VA_ARGS__)

[[noreturn]]
void todo_impl(const char *file, size_t line, const char *fmt, va_list args) {
	fprintf(stderr, "TODO @ %s:%zu: ", file, line);
	vfprintf(stderr, fmt, args);
	fprintf(stderr, "\n");

	va_end(args);
	exit(EXIT_FAILURE);
}

[[noreturn]]
void todo_impl(const char *file, size_t line, const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);

	todo_impl(file, line, fmt, args);
}

template<typename Ok, typename Err = std::string>
class Result {
	std::variant<Ok, Err> repr;

public:
	Result(const Ok& ok) : 
		repr(ok) 
	{
	}

	Result(const Err& err) :
		repr(err)
	{
	}

	const Ok& unwrap() {
		if (std::holds_alternative<Err>(repr)) {
			std::cerr << std::get<Err>(repr);
			exit(EXIT_FAILURE);
		}
		return std::get<Ok>(repr);
	}

	bool is_ok() {
		return std::holds_alternative<Ok>(repr);
	}

	bool is_err() {
		return std::holds_alternative<Err>(repr);
	}

	const Ok& ok() {
		return std::get<Ok>(repr);
	}

	const Err& err() {
		return std::get<std::string>(repr);
	}
};

template<typename Ok>
class Result<Ok, void> {
	bool _is_ok;
	Ok _ok;

public:
	Result() : 
		_is_ok(false) 
	{
	}

	Result(const Ok& ok) : 
		_is_ok(false),
		_ok(ok)
	{
	}

	const Ok& unwrap() {
		if (!_is_ok) {
			std::cerr << "Error: Attempted unwrap of not-ok Result!" << std::endl;
			exit(EXIT_FAILURE);
		}
		return _ok;
	}

	bool is_ok() {
		return _is_ok;
	}

	bool is_err() {
		return !_is_ok;
	}

	const Ok& ok() {
		return _ok;
	}
};

template<typename Err>
class Result<void, Err> {
	bool _is_ok;
	Err _err;

public:
	Result() : _is_ok(true) { }

	Result(const Err& err) : 
		_is_ok(false),
		_err(err) 
	{
	}

	void unwrap() {
		if (!_is_ok) {
			std::cerr << _err;
			exit(EXIT_FAILURE);
		}
	}

	bool is_ok() {
		return _is_ok;
	}

	bool is_err() {
		return !_is_ok;
	}

	const Err& err() {
		return _err;
	}
};

template<>
class Result<void, void> {
	bool _is_ok;

public:
	Result(bool is_ok) :
		_is_ok(is_ok) 
	{
	}

	void unwrap() {
		if (!_is_ok) {
			std::cerr << "Error: Attempted unwrap of not-ok Result!" << std::endl;
			exit(EXIT_FAILURE);
		}
	}

	bool is_ok() {
		return _is_ok;
	}

	bool is_err() {
		return !_is_ok;
	}
};

#define try_(expression) ({\
	auto result = expression;\
	if (result.is_err()) return { std::move(result.err()) };\
	result.ok();\
})

#define error(...) ({\
	auto result = error_impl(__FILE__, __LINE__, __VA_ARGS__);\
	assert(result.is_err());\
	return { std::move(result.err()) };\
})

Result<void> error_impl(const char *file, int line, const char *err, va_list args) {
	std::stringstream s;

	char *message;
	vasprintf(&message, err, args);

	s << "Error @ " << file << ':' << line << ": " << message << std::endl;

	free(message);
	va_end(args);

	return { s.str() };
}

Result<void> error_impl(const char *file, int line, const char *err, ...) {
	va_list args;
	va_start(args, err);

	return error_impl(file, line, err, args);
}

#define verify(condition, ...) if (!(condition)) error(__VA_ARGS__)

