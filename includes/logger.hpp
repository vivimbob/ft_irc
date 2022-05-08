#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <cstdarg>

namespace logger
{
    enum
    {
        LogLevel_Verbose,
        LogLevel_Debug,
        LogLevel_Info,
        LogLevel_Warning,
        LogLevel_Error,
        LogLevel_Fatal
    };

    /**
     * @brief VERBOSE 등급으로 로그를 출력합니다. 가장 낮은 로깅 등급입니다. printf와 사용법이 동일합니다.
     *
     * @param format 형식 타입
     * @param ... 메시지 인자
     */
    void V(const char* format, ...);

    /**
     * @brief DEBUG 등급으로 로그를 출력합니다. 2번째로 낮은 로깅 등급입니다. printf와 사용법이 동일합니다.
     *
     * @param format 형식 타입
     * @param ... 메시지 인자
     */
    void D(const char* format, ...);

    /**
     * @brief INFO 등급으로 로그를 출력합니다. 3번째로 낮은 로깅 등급입니다. printf와 사용법이 동일합니다.
     *
     * @param format 형식 타입
     * @param ... 메시지 인자
     */
    void I(const char* format, ...);

    /**
     * @brief WARNING 등급으로 로그를 출력합니다. 3번째로 높은 로깅 등급입니다. printf와 사용법이 동일합니다.
     *
     * @param format 형식 타입
     * @param ... 메시지 인자
     */
    void W(const char* format, ...);

    /**
     * @brief ERROR 등급으로 로그를 출력합니다. 2번째로 높은 로깅 등급입니다. printf와 사용법이 동일합니다.
     *
     * @param format 형식 타입
     * @param ... 메시지 인자
     */
    void E(const char* format, ...);

    /**
     * @brief FATAL 등급으로 로그를 출력합니다. 가장 높은 로깅 등급입니다. printf와 사용법이 동일합니다.
     *
     * @param format 형식 타입
     * @param ... 메시지 인자
     */
    void F(const char* format, ...);

    /**
     * @brief VERBOSE 등급으로 Prefix 열에 내용을 채워 로그를 출력합니다. 가장 낮은 로깅 등급입니다. printf와 사용법이 동일합니다.
     *
     * @param prefix 로그 앞에 붙일 머릿말
     * @param format 형식 타입
     * @param ... 메시지 인자
     */
    void Vp(const char* prefix, const char* format, ...);

    /**
     * @brief DEBUG 등급으로 Prefix 열에 내용을 채워 로그를 출력합니다. 2번째로 낮은 로깅 등급입니다. printf와 사용법이 동일합니다.
     *
     * @param prefix 로그 앞에 붙일 머릿말
     * @param format 형식 타입
     * @param ... 메시지 인자
     */
    void Dp(const char* prefix, const char* format, ...);

    /**
     * @brief INFO 등급으로 Prefix 열에 내용을 채워 로그를 출력합니다. 3번째로 낮은 로깅 등급입니다. printf와 사용법이 동일합니다.
     *
     * @param prefix 로그 앞에 붙일 머릿말
     * @param format 형식 타입
     * @param ... 메시지 인자
     */
    void Ip(const char* prefix, const char* format, ...);

    /**
     * @brief WARNING 등급으로 Prefix 열에 내용을 채워 로그를 출력합니다. 3번째로 높은 로깅 등급입니다. printf와 사용법이 동일합니다.
     *
     * @param prefix 로그 앞에 붙일 머릿말
     * @param format 형식 타입
     * @param ... 메시지 인자
     */
    void Wp(const char* prefix, const char* format, ...);

    /**
     * @brief ERROR 등급으로 Prefix 열에 내용을 채워 로그를 출력합니다. 2번째로 높은 로깅 등급입니다. printf와 사용법이 동일합니다.
     *
     * @param prefix 로그 앞에 붙일 머릿말
     * @param format 형식 타입
     * @param ... 메시지 인자
     */
    void Ep(const char* prefix, const char* format, ...);

    /**
     * @brief FATAL 등급으로 Prefix 열에 내용을 채워 로그를 출력합니다. 가장 높은 로깅 등급입니다. printf와 사용법이 동일합니다.
     *
     * @param prefix 로그 앞에 붙일 머릿말
     * @param format 형식 타입
     * @param ... 메시지 인자
     */
    void Fp(const char* prefix, const char* format, ...);
};  // namespace space

#endif