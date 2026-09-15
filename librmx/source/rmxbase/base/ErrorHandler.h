/*
*	rmx Library
*	Copyright (C) 2008-2024 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#pragma once


#include <sstream>
#include <stdexcept> // for std::runtime_error


// Debug break (platform specific)
#ifdef _MSC_VER
	#define RMX_DEBUG_BREAK	__debugbreak()
#elif __APPLE__
	#define RMX_DEBUG_BREAK __builtin_trap()
#elif defined(__ANDROID__)
	#define RMX_DEBUG_BREAK assert(false)
#else
	#define RMX_DEBUG_BREAK {}
#endif


// Asserts
#define RMX_CONDITIONAL_ERROR(severity, condition, explanation, reaction) \
{ \
	if (!(condition)) \
	{ \
		std::ostringstream _stream_; \
		_stream_ << explanation; \
		if (rmx::ErrorHandling::handleAssertBreak(severity, _stream_.str(), __FILE__, __LINE__)) \
			RMX_DEBUG_BREAK; \
		reaction; \
	} \
}

#ifdef PLATFORM_WINDOWS
	#define RMX_REACT_THROW throw std::runtime_error(_stream_.str())
#else
	#define RMX_REACT_THROW
#endif

#if defined(PLATFORM_PS3)
	#define RMX_SEVERITY_ERROR rmx::ErrorSeverity::ERROR
#else
	#define RMX_SEVERITY_ERROR rmx::ErrorSeverity::ERROR
#endif

#ifdef DEBUG
	#define RMX_ASSERT(condition, message)		RMX_CONDITIONAL_ERROR(RMX_SEVERITY_ERROR, condition, message, )
#else
	#define RMX_ASSERT(condition, message)		{}
#endif

#define RMX_CHECK(condition, message, reaction)	RMX_CONDITIONAL_ERROR(RMX_SEVERITY_ERROR, condition, message, reaction)
#define RMX_ERROR(message, reaction)			RMX_CONDITIONAL_ERROR(RMX_SEVERITY_ERROR, false, message, reaction)


namespace rmx
{
#if defined(PLATFORM_PS3)
	struct ErrorSeverity
	{
		enum Enum
		{
			INFO,
			WARNING,
			ERROR
		};
	};
	typedef ErrorSeverity::Enum ErrorSeverity_t;
#else
	enum class ErrorSeverity
	{
		INFO,
		WARNING,
		ERROR
	};
	using ErrorSeverity_t = ErrorSeverity;
#endif

	struct ErrorHandling
	{
	public:
		class LoggerInterface
		{
		public:
			virtual ~LoggerInterface() {}
			virtual void logMessage(ErrorSeverity_t errorSeverity, const std::string& message) = 0;
		};

		class MessageBoxInterface
		{
		public:
#if defined(PLATFORM_PS3)
			struct DialogType
			{
				enum Enum
				{
					ACCEPT_ONLY,
					ACCEPT_OR_CANCEL,
					ALL_OPTIONS
				};
			};
			typedef DialogType::Enum DialogType_t;

			struct Result
			{
				enum Enum
				{
					ACCEPT,
					ABORT,
					IGNORE
				};
			};
			typedef Result::Enum Result_t;
#else
			enum class DialogType
			{
				ACCEPT_ONLY,
				ACCEPT_OR_CANCEL,
				ALL_OPTIONS
			};
			using DialogType_t = DialogType;

			enum class Result
			{
				ACCEPT,
				ABORT,
				IGNORE
			};
			using Result_t = Result;
#endif

		public:
			virtual ~MessageBoxInterface() {}
			virtual Result_t showMessageBox(DialogType_t dialogType, ErrorSeverity_t errorSeverity, const std::string& message, const char* filename, int line) = 0;
		};

	public:
		static bool isDebuggerAttached();
		static void printToLog(ErrorSeverity_t errorSeverity, const std::string& message);
		static bool handleAssertBreak(ErrorSeverity_t errorSeverity, const std::string& message, const char* filename, int line);

	public:
		#if defined(PLATFORM_PS3)
		static LoggerInterface* mLogger;
		static MessageBoxInterface* mMessageBoxImplementation;
		#else
		static LoggerInterface* mLogger;
		static MessageBoxInterface* mMessageBoxImplementation;
		#endif
	};
}
