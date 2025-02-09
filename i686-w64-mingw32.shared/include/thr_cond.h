#ifndef THR_COND_INCLUDED
#define THR_COND_INCLUDED

/* Copyright (c) 2014, Oracle and/or its affiliates. All rights reserved.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA */

/**
  MySQL condition variable implementation.

  There are three "layers":
  1) native_cond_*()
       Functions that map directly down to OS primitives.
       Windows    - ConditionVariable
       Other OSes - pthread
  2) my_cond_*()
       Functions that use SAFE_MUTEX (default for debug).
       FAST_MUTEX (default for release - non Windows). If neither
       of these apply, native_cond_*() is used.
  3) mysql_cond*()
       Functions that include Performance Schema instrumentation.
       See include/mysql/psi/mysql_thread.h
*/

#ifdef _WIN32
typedef CONDITION_VARIABLE native_cond_t;
#else
typedef pthread_cond_t native_cond_t;
#endif

#ifdef _MSC_VER
/**
  Convert abstime to milliseconds
*/

static DWORD get_milliseconds(const struct timespec *abstime)
{
  long long millis;
  union ft64 now;

  if (abstime == NULL)
   return INFINITE;

  GetSystemTimeAsFileTime(&now.ft);

  /*
    Calculate time left to abstime
    - subtract start time from current time(values are in 100ns units)
    - convert to millisec by dividing with 10000
  */
  millis= (abstime->tv.i64 - now.i64) / 10000;

  /* Don't allow the timeout to be negative */
  if (millis < 0)
    return 0;

  /*
    Make sure the calculated timeout does not exceed original timeout
    value which could cause "wait for ever" if system time changes
  */
  if (millis > abstime->max_timeout_msec)
    millis= abstime->max_timeout_msec;

  if (millis > UINT_MAX)
    millis= UINT_MAX;

  return (DWORD)millis;
}
#endif /* _WIN32 */

static inline int native_cond_init(native_cond_t *cond)
{
#ifdef _WIN32
  InitializeConditionVariable(cond);
  return 0;
#else
  /* pthread_condattr_t is not used in MySQL */
  return pthread_cond_init(cond, NULL);
#endif
}

static inline int native_cond_destroy(native_cond_t *cond)
{
#ifdef _WIN32
  return 0; /* no destroy function */
#else
  return pthread_cond_destroy(cond);
#endif
}

static inline int native_cond_timedwait(native_cond_t *cond,
                                        native_mutex_t *mutex,
                                        const struct timespec *abstime)
{
#ifdef _WIN32
  DWORD timeout= abstime->tv_nsec / 1000000 + abstime->tv_sec * 1000;
  if (!SleepConditionVariableCS(cond, mutex, timeout))
    return ETIMEDOUT;
  return 0;
#else
  return pthread_cond_timedwait(cond, mutex, abstime);
#endif
}

static inline int native_cond_wait(native_cond_t *cond, native_mutex_t *mutex)
{
#ifdef _WIN32
  return native_cond_timedwait(cond, mutex, NULL);
#else
  return pthread_cond_wait(cond, mutex);
#endif
}

static inline int native_cond_signal(native_cond_t *cond)
{
#ifdef _WIN32
  WakeConditionVariable(cond);
  return 0;
#else
  return pthread_cond_signal(cond);
#endif
}

static inline int native_cond_broadcast(native_cond_t *cond)
{
#ifdef _WIN32
  WakeAllConditionVariable(cond);
  return 0;
#else
  return pthread_cond_broadcast(cond);
#endif
}

#ifdef SAFE_MUTEX
int safe_cond_wait(native_cond_t *cond, my_mutex_t *mp,
                   const char *file, uint line);
int safe_cond_timedwait(native_cond_t *cond, my_mutex_t *mp,
                        const struct timespec *abstime,
                        const char *file, uint line);
#endif

static inline int my_cond_timedwait(native_cond_t *cond, my_mutex_t *mp,
                                    const struct timespec *abstime
#ifdef SAFE_MUTEX
                                    , const char *file, uint line
#endif
                                    )
{
#ifdef SAFE_MUTEX
  return safe_cond_timedwait(cond, mp, abstime, file, line);
#elif defined MY_PTHREAD_FASTMUTEX
  return native_cond_timedwait(cond, &mp->mutex, abstime);
#else
  return native_cond_timedwait(cond, mp, abstime);
#endif
}

static inline int my_cond_wait(native_cond_t *cond, my_mutex_t *mp
#ifdef SAFE_MUTEX
                               , const char *file, uint line
#endif
                               )
{
#ifdef SAFE_MUTEX
  return safe_cond_wait(cond, mp, file, line);
#elif defined MY_PTHREAD_FASTMUTEX
  return native_cond_wait(cond, &mp->mutex);
#else
  return native_cond_wait(cond, mp);
#endif
}

#endif /* THR_COND_INCLUDED */
