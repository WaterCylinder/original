#ifndef ORIGINAL_ZEIT_H
#define ORIGINAL_ZEIT_H
#include <cmath>
#include "config.h"
#include "comparable.h"
#include "hash.h"
#include "printable.h"
#include "error.h"
#include <iomanip>
#include <chrono>


namespace original {
    class time final {
    public:
        using time_val_type = integer;
    private:
        static constexpr time_val_type UNIT_FACTOR_BASE = 1;

        static constexpr time_val_type UNIT_FACTOR[] {
                UNIT_FACTOR_BASE,
                UNIT_FACTOR_BASE * 1000,
                UNIT_FACTOR_BASE * 1000 * 1000,
                UNIT_FACTOR_BASE * 1000 * 1000 * 1000,
                UNIT_FACTOR_BASE * 1000 * 1000 * 1000 * 60,
                UNIT_FACTOR_BASE * 1000 * 1000 * 1000 * 60 * 60,
                UNIT_FACTOR_BASE * 1000 * 1000 * 1000 * 60 * 60 * 24,
        };
    public:
        enum class unit {
            NANOSECOND,
            MICROSECOND,
            MILLISECOND,
            SECOND,
            MINUTE,
            HOUR,
            DAY,
        };

        static constexpr auto NANOSECOND = unit::NANOSECOND;
        static constexpr auto MICROSECOND = unit::MICROSECOND;
        static constexpr auto MILLISECOND = unit::MILLISECOND;
        static constexpr auto SECOND = unit::SECOND;
        static constexpr auto MINUTE = unit::MINUTE;
        static constexpr auto HOUR = unit::HOUR;
        static constexpr auto DAY = unit::DAY;

        static constexpr time_val_type FACTOR_NANOSECOND = UNIT_FACTOR[static_cast<ul_integer>(NANOSECOND)];
        static constexpr time_val_type FACTOR_MICROSECOND = UNIT_FACTOR[static_cast<ul_integer>(MICROSECOND)];
        static constexpr time_val_type FACTOR_MILLISECOND = UNIT_FACTOR[static_cast<ul_integer>(MILLISECOND)];
        static constexpr time_val_type FACTOR_SECOND = UNIT_FACTOR[static_cast<ul_integer>(SECOND)];
        static constexpr time_val_type FACTOR_MINUTE = UNIT_FACTOR[static_cast<ul_integer>(MINUTE)];
        static constexpr time_val_type FACTOR_HOUR = UNIT_FACTOR[static_cast<ul_integer>(HOUR)];
        static constexpr time_val_type FACTOR_DAY = UNIT_FACTOR[static_cast<ul_integer>(DAY)];

        static constexpr integer DAYS_LEAP_YEAR = 366;
        static constexpr integer DAYS_COMMON_YEAR = 365;

        static constexpr integer EPOCH_YEAR = 1970;
        static constexpr integer EPOCH_MONTH = 1;
        static constexpr integer EPOCH_DAY = 1;

        class point;
        class UTCTime;

        class duration final
                      : public comparable<duration>,
                        public hashable<duration>,
                        public printable {
            time_val_type nano_seconds_;

        public:
            friend class point;

            static const duration ZERO;

            explicit duration(time_val_type val = 0, unit unit = MILLISECOND);

            duration(const duration& other) = default;

            duration& operator=(const duration& other) = default;

            duration(duration&& other) noexcept;

            duration& operator=(duration&& other) noexcept;

            [[nodiscard]] time_val_type value(unit unit = MILLISECOND) const noexcept;

            [[nodiscard]] integer compareTo(const duration& other) const override;

            u_integer toHash() const noexcept override;

            std::string className() const override;

            std::string toString(bool enter) const override;

            duration& operator++();

            duration operator++(int postfix);

            duration& operator--();

            duration operator--(int postfix);

            duration& operator+=(const duration& other);

            duration& operator-=(const duration& other);

            duration& operator*=(time_val_type factor);

            duration& operator/=(time_val_type factor);

            duration& operator/=(const duration& other);

            [[nodiscard]] floating div(time_val_type factor, unit unit = MILLISECOND) const;

            [[nodiscard]] floating div(const duration& other) const;

            friend duration operator-(const duration& d);

            friend duration operator+(const duration& lhs, const duration& rhs);

            friend duration operator-(const duration& lhs, const duration& rhs);

            friend duration operator*(const duration& d, time_val_type factor);

            friend duration operator*(time_val_type factor, const duration& d);

            friend duration operator/(const duration& d, time_val_type factor);

            friend duration operator/(const duration& lhs, const duration& rhs);

            friend duration abs(const duration& d);
        };

        class point final
                : public comparable<point>,
                  public hashable<point>,
                  public printable {
            duration nano_since_epoch_;

        public:
            friend class UTCTime;

            static point now();

            explicit point(time_val_type val = 0, unit unit = MILLISECOND);

            explicit point(duration d);

            [[nodiscard]] time_val_type value(unit unit = MILLISECOND) const noexcept;

            [[nodiscard]] integer compareTo(const point& other) const override;

            u_integer toHash() const noexcept override;

            std::string className() const override;

            std::string toString(bool enter) const override;

            point& operator++();

            point operator++(int postfix);

            point& operator--();

            point operator--(int postfix);

            point& operator+=(const duration& d);

            point& operator-=(const duration& d);

            friend point operator+(const point& p, const duration& d);

            friend point operator-(const point& p, const duration& d);

            friend duration operator-(const point& lhs, const point& rhs);
        };

        class UTCTime final
                : public comparable<UTCTime>,
                  public hashable<UTCTime>,
                  public printable {

            integer year_{};
            integer month_{};
            integer day_{};
            integer hour_{};
            integer minute_{};
            integer second_{};

            enum class calendar : integer {
                YEAR,
                MONTH,
            };

            enum class weekdays : integer {
                SATURDAY,
                SUNDAY,
                MONDAY,
                TUESDAY,
                WEDNESDAY,
                THURSDAY,
                FRIDAY,
            };

            static constexpr integer DAYS_OF_MONTH[] {
                    31, 28, 31, 30, 31, 30,
                    31, 31, 30, 31, 30, 31
            };

            static constexpr bool isValidYear(integer year);

            static constexpr bool isValidMonth(integer month);

            static constexpr bool isValidDay(integer day);

            void set(integer year, integer month, integer day,
                     integer hour, integer minute, integer second);
        public:
            friend time;

            static constexpr auto SATURDAY = weekdays::SATURDAY;
            static constexpr auto SUNDAY = weekdays::SUNDAY;
            static constexpr auto MONDAY = weekdays::MONDAY;
            static constexpr auto TUESDAY = weekdays::TUESDAY;
            static constexpr auto WEDNESDAY = weekdays::WEDNESDAY;
            static constexpr auto THURSDAY = weekdays::THURSDAY;
            static constexpr auto FRIDAY = weekdays::FRIDAY;


            static constexpr integer DAYS_WEEK = 7;
            static constexpr integer MONTHS_YEAR = std::size(DAYS_OF_MONTH);
            static constexpr integer YEARS_CENTURY = 100;

            static const UTCTime EPOCH;

            static UTCTime now();

            static constexpr bool isLeapYear(integer year);

            static constexpr integer daysOfMonth(integer year, integer month);

            static constexpr weekdays weekday(integer year, integer month, integer day);

            static constexpr bool isValidYMD(integer year, integer month, integer day);

            static constexpr bool isValidHMS(integer hour, integer minute, integer second);

            static constexpr bool isValid(integer year, integer month, integer day,
                                          integer hour, integer minute, integer second);

            [[nodiscard]] bool isLeapYear() const;

            [[nodiscard]] weekdays weekday() const;

            explicit UTCTime();

            explicit UTCTime(integer year, integer month, integer day,
                             integer hour, integer minute, integer second);

            explicit UTCTime(const point& p);

            UTCTime(const UTCTime& other) = default;

            UTCTime& operator=(const UTCTime& other) = default;

            UTCTime(UTCTime&& other) noexcept;

            UTCTime& operator=(UTCTime&& other) noexcept;

            integer value(unit unit) const noexcept;

            integer value(calendar calendar) const noexcept;

            explicit operator point() const;

            integer compareTo(const UTCTime& other) const override;

            u_integer toHash() const noexcept override;

            std::string className() const override;

            std::string toString(bool enter) const override;

            friend UTCTime operator+(const UTCTime& t, const duration& d);

            friend UTCTime operator-(const UTCTime& t, const duration& d);

            friend duration operator-(const UTCTime& lhs, const UTCTime& rhs);
        };

        static constexpr auto MONTH = UTCTime::calendar::MONTH;
        static constexpr auto YEAR = UTCTime::calendar::YEAR;
    };

    const time::duration time::duration::ZERO = duration{};

    time::duration operator-(const time::duration& d);

    time::duration operator+(const time::duration& lhs, const time::duration& rhs);

    time::duration operator-(const time::duration& lhs, const time::duration& rhs);

    time::duration operator*(const time::duration& d, time::time_val_type factor);

    time::duration operator*(time::time_val_type factor, const time::duration& d);

    time::duration operator/(const time::duration& d, time::time_val_type factor);

    time::duration operator/(const time::duration& lhs, const time::duration& rhs);

    time::duration abs(const time::duration& d);

    time::point operator+(const time::point& p, const time::duration& d);

    time::point operator-(const time::point& p, const time::duration& d);

    time::duration operator-(const time::point& lhs, const time::point& rhs);

    const time::UTCTime time::UTCTime::EPOCH = UTCTime{};

    time::UTCTime operator+(const time::UTCTime& t, const time::duration& d);

    time::UTCTime operator-(const time::UTCTime& t, const time::duration& d);

    time::duration operator-(const time::UTCTime& lhs, const time::UTCTime& rhs);

    namespace literals {
        inline time::duration operator""_ns(const unsigned long long val) {
            return time::duration(static_cast<time::time_val_type>(val), time::NANOSECOND);
        }

        inline time::duration operator""_us(const unsigned long long val) {
            return time::duration(static_cast<time::time_val_type>(val), time::MICROSECOND);
        }

        inline time::duration operator""_ms(const unsigned long long val) {
            return time::duration(static_cast<time::time_val_type>(val), time::MILLISECOND);
        }

        inline time::duration operator""_s(const unsigned long long val) {
            return time::duration(static_cast<time::time_val_type>(val), time::SECOND);
        }

        inline time::duration operator""_min(const unsigned long long val) {
            return time::duration(static_cast<time::time_val_type>(val), time::MINUTE);
        }

        inline time::duration operator""_h(const unsigned long long val) {
            return time::duration(static_cast<time::time_val_type>(val), time::HOUR);
        }

        inline time::duration operator""_d(const unsigned long long val) {
            return time::duration(static_cast<time::time_val_type>(val), time::DAY);
        }

        inline time::duration operator""_ns(const long double val) {
            return time::duration(std::llround(val * time::FACTOR_NANOSECOND), time::NANOSECOND);
        }

        inline time::duration operator""_us(const long double val) {
            return time::duration(std::llround(val * time::FACTOR_MICROSECOND), time::NANOSECOND);
        }

        inline time::duration operator""_ms(const long double val) {
            return time::duration(std::llround(val * time::FACTOR_MILLISECOND), time::NANOSECOND);
        }

        inline time::duration operator""_s(const long double val) {
            return time::duration(std::llround(val * time::FACTOR_SECOND), time::NANOSECOND);
        }

        inline time::duration operator""_min(const long double val) {
            return time::duration(std::llround(val * time::FACTOR_MINUTE), time::NANOSECOND);
        }

        inline time::duration operator""_h(const long double val) {
            return time::duration(std::llround(val * time::FACTOR_HOUR), time::NANOSECOND);
        }

        inline time::duration operator""_d(const long double val) {
            return time::duration(std::llround(val * time::FACTOR_DAY), time::NANOSECOND);
        }
    }
}

inline original::time::duration::duration(const time_val_type val, const unit unit) {
    switch (unit) {
        case unit::DAY:
            this->nano_seconds_ = FACTOR_DAY * val;
            break;
        case unit::HOUR:
            this->nano_seconds_ = FACTOR_HOUR * val;
            break;
        case unit::MINUTE:
            this->nano_seconds_ = FACTOR_MINUTE * val;
            break;
        case unit::SECOND:
            this->nano_seconds_ = FACTOR_SECOND * val;
            break;
        case unit::MILLISECOND:
            this->nano_seconds_ = FACTOR_MILLISECOND * val;
            break;
        case unit::MICROSECOND:
            this->nano_seconds_ = FACTOR_MICROSECOND * val;
            break;
        case unit::NANOSECOND:
            this->nano_seconds_ = FACTOR_NANOSECOND * val;
    }
}

inline original::time::duration::duration(duration&& other) noexcept : duration() {
    this->operator=(std::move(other));
}

inline original::time::duration&
original::time::duration::operator=(duration&& other) noexcept {
    if (this == &other)
        return *this;

    this->nano_seconds_ = other.nano_seconds_;
    other.nano_seconds_ = 0;
    return *this;
}

inline original::time::time_val_type
original::time::duration::value(const unit unit) const noexcept {
    time_val_type val = this->nano_seconds_;
    switch (unit) {
        case unit::DAY:
            val /= FACTOR_DAY;
            break;
        case unit::HOUR:
            val /= FACTOR_HOUR;
            break;
        case unit::MINUTE:
            val /= FACTOR_MINUTE;
            break;
        case unit::SECOND:
            val /= FACTOR_SECOND;
            break;
        case unit::MILLISECOND:
            val /= FACTOR_MILLISECOND;
            break;
        case unit::MICROSECOND:
            val /= FACTOR_MICROSECOND;
            break;
        case unit::NANOSECOND:
            val /= FACTOR_NANOSECOND;
    }
    return val;
}

inline original::integer
original::time::duration::compareTo(const duration& other) const {
    if (this->nano_seconds_ == other.nano_seconds_)
        return 0;
    return this->nano_seconds_ > other.nano_seconds_ ? 1 : -1;
}

inline original::u_integer
original::time::duration::toHash() const noexcept {
    return hash<time_val_type>::hashFunc(this->nano_seconds_);
}

inline std::string original::time::duration::className() const {
    return "time::duration";
}

inline std::string original::time::duration::toString(const bool enter) const {
    std::stringstream ss;
    ss << "(" << this->className() << " " << this->nano_seconds_ << "ns)";
    if (enter)
        ss << "\n";
    return ss.str();
}

inline original::time::duration&
original::time::duration::operator++() {
    this->nano_seconds_ += 1;
    return *this;
}

inline original::time::duration
original::time::duration::operator++(int) {
    duration res{*this};
    this->nano_seconds_ += 1;
    return res;
}

inline original::time::duration&
original::time::duration::operator--() {
    this->nano_seconds_ -= 1;
    return *this;
}

inline original::time::duration
original::time::duration::operator--(int) {
    duration res(*this);
    this->nano_seconds_ -= 1;
    return res;
}

inline original::time::duration&
original::time::duration::operator+=(const duration& other) {
    this->nano_seconds_ += other.nano_seconds_;
    return *this;
}

inline original::time::duration&
original::time::duration::operator-=(const duration& other) {
    this->nano_seconds_ -= other.nano_seconds_;
    return *this;
}

inline original::time::duration&
original::time::duration::operator*=(const time_val_type factor) {
    this->nano_seconds_ *= factor;
    return *this;
}

inline original::time::duration&
original::time::duration::operator/=(const time_val_type factor) {
    this->nano_seconds_ /= factor;
    return *this;
}

inline original::time::duration&
original::time::duration::operator/=(const duration& other) {
    this->nano_seconds_ /= other.nano_seconds_;
    return *this;
}

inline original::floating
original::time::duration::div(const time_val_type factor, const unit unit) const {
    return this->div(duration{factor, unit});
}

inline original::floating
original::time::duration::div(const duration& other) const {
    return static_cast<floating>(this->nano_seconds_) / static_cast<floating>(other.nano_seconds_);
}

inline original::time::duration
original::operator-(const time::duration& d) {
    time::duration res{d};
    res.nano_seconds_ = -res.nano_seconds_;
    return res;
}

inline original::time::duration
original::operator+(const time::duration& lhs, const time::duration& rhs) {
    time::duration d{lhs};
    d += rhs;
    return d;
}

inline original::time::duration
original::operator-(const time::duration& lhs, const time::duration& rhs) {
    time::duration d{lhs};
    d -= rhs;
    return d;
}

inline original::time::duration
original::operator*(const time::duration &d, const time::time_val_type factor) {
    time::duration res{d};
    res *= factor;
    return res;
}

inline original::time::duration
original::operator*(const time::time_val_type factor, const time::duration& d) {
    time::duration res{d};
    res *= factor;
    return res;
}

inline original::time::duration
original::operator/(const time::duration &d, const time::time_val_type factor) {
    time::duration res{d};
    res /= factor;
    return res;
}

inline original::time::duration
original::operator/(const time::duration& lhs, const time::duration& rhs) {
    time::duration d{lhs};
    d /= rhs;
    return d;
}

inline original::time::duration
original::abs(const time::duration& d) {
    time::time_val_type val = d.nano_seconds_;
    val = val < 0 ? -val : val;
    return time::duration{val, time::NANOSECOND};
}

inline original::time::point
original::time::point::now() {
#if ORIGINAL_COMPILER_GCC || (ORIGINAL_COMPILER_CLANG && ORIGINAL_PLATFORM_LINUX)
    timespec ts{};
    clock_gettime(CLOCK_REALTIME, &ts);
    time_val_type ns = ts.tv_sec * FACTOR_SECOND + ts.tv_nsec;
    return point(ns, NANOSECOND);
#elif ORIGINAL_PLATFORM_APPLE
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    time_val_type ns = tv.tv_sec * FACTOR_SECOND + tv.tv_usec * 1000;
    return point(ns, NANOSECOND);
#else
    // Other implements not complete
    return point();
#endif
}

inline original::time::point::point(const time_val_type val, const unit unit)
    : nano_since_epoch_(val, unit) {}

inline original::time::point::point(duration d)
    : nano_since_epoch_(std::move(d)) {}

inline original::time::time_val_type
original::time::point::value(const unit unit) const noexcept {
    return this->nano_since_epoch_.value(unit);
}

inline original::integer
original::time::point::compareTo(const point& other) const {
    return this->nano_since_epoch_.compareTo(other.nano_since_epoch_);
}

inline original::u_integer
original::time::point::toHash() const noexcept {
    return this->nano_since_epoch_.toHash();
}

inline std::string original::time::point::className() const {
    return "time::point";
}

inline std::string
original::time::point::toString(const bool enter) const {
    std::stringstream ss;
    ss << "(" << this->className() << " " << this->nano_since_epoch_.nano_seconds_ << ")";
    if (enter)
        ss << "\n";
    return ss.str();
}

inline original::time::point&
original::time::point::operator++() {
    ++this->nano_since_epoch_;
    return *this;
}

inline original::time::point
original::time::point::operator++(int) {
    point res{*this};
    ++this->nano_since_epoch_;
    return res;
}

inline original::time::point&
original::time::point::operator--() {
    --this->nano_since_epoch_;
    return *this;
}

inline original::time::point
original::time::point::operator--(int) {
    point res{*this};
    --this->nano_since_epoch_;
    return res;
}

inline original::time::point&
original::time::point::operator+=(const duration& d) {
    this->nano_since_epoch_ += d;
    return *this;
}

inline original::time::point&
original::time::point::operator-=(const duration& d) {
    this->nano_since_epoch_ -= d;
    return *this;
}

inline original::time::point
original::operator-(const time::point &p, const time::duration &d) {
    time::point res{p};
    res -= d;
    return res;
}

inline original::time::duration
original::operator-(const time::point &lhs, const time::point &rhs) {
    return lhs.nano_since_epoch_ - rhs.nano_since_epoch_;
}

inline original::time::point
original::operator+(const time::point &p, const time::duration &d) {
    time::point res{p};
    res += d;
    return res;
}

constexpr bool
original::time::UTCTime::isValidYear(const integer year) {
    return 0 <= year;
}

constexpr bool
original::time::UTCTime::isValidMonth(const integer month) {
    return 1 <= month && month <= 12;
}

constexpr bool
original::time::UTCTime::isValidDay(const integer day) {
    return 1 <= day && day <= 31;
}

inline void original::time::UTCTime::set(const integer year, const integer month, const integer day,
                                         const integer hour, const integer minute, const integer second) {
    this->year_ = year;
    this->month_ = month;
    this->day_ = day;
    this->hour_ = hour;
    this->minute_ = minute;
    this->second_ = second;
}

inline original::time::UTCTime
original::time::UTCTime::now() {
    return UTCTime{point::now()};
}

constexpr bool
original::time::UTCTime::isLeapYear(const integer year) {
    if (!isValidYear(year))
        return false;
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

constexpr original::integer
original::time::UTCTime::daysOfMonth(const integer year, const integer month) {
    if (!isValidYear(year) || !isValidMonth(month))
        throw valueError();

    if (month == 2 && isLeapYear(year)){
        return DAYS_OF_MONTH[month - 1] + 1;
    }
    return DAYS_OF_MONTH[month - 1];
}

constexpr original::time::UTCTime::weekdays
original::time::UTCTime::weekday(const integer year, const integer month, const integer day) {
    if (!isValidYMD(year, month, day))
        throw valueError();

    integer corrected_year, corrected_month;
    if (month == 1 || month == 2){
        corrected_year = year - 1;
        corrected_month = MONTHS_YEAR + month;
    } else {
        corrected_year = year;
        corrected_month = month;
    }
    const integer century = corrected_year / YEARS_CENTURY;
    const integer years_in_century = corrected_year % YEARS_CENTURY;

    return static_cast<weekdays>((
               day +
               13 * (corrected_month + 1) / 5 +
               years_in_century +
               years_in_century / 4 +
               century / 4 +
               5 * century
           ) % DAYS_WEEK);
}

constexpr bool
original::time::UTCTime::isValidYMD(const integer year, const integer month, const integer day) {
    if (!isValidYear(year) || !isValidMonth(month) || !isValidDay(day))
        return false;

    return day <= daysOfMonth(year, month);
}

constexpr bool
original::time::UTCTime::isValidHMS(const integer hour, const integer minute, const integer second) {
    return 0 <= hour && hour <= 23 &&
           0 <= minute && minute <= 59 &&
           0 <= second && second <= 59;
}

constexpr bool
original::time::UTCTime::isValid(const integer year, const integer month, const integer day,
                                 const integer hour, const integer minute, const integer second) {
    return isValidYMD(year, month, day) && isValidHMS(hour, minute, second);
}

inline bool original::time::UTCTime::isLeapYear() const {
    return isLeapYear(this->year_);
}

inline original::time::UTCTime::weekdays
original::time::UTCTime::weekday() const {
    return weekday(this->year_, this->month_, this->day_);
}

inline original::time::UTCTime::UTCTime()
    : UTCTime(EPOCH_YEAR, EPOCH_MONTH, EPOCH_DAY, 0, 0, 0) {}

inline original::time::UTCTime::UTCTime(const integer year, const integer month, const integer day,
                                        const integer hour, const integer minute, const integer second) {
    if (!isValid(year, month, day, hour, minute, second))
        throw valueError();

    this->set(year, month, day, hour, minute, second);
}

inline original::time::UTCTime::UTCTime(const point& p) {
    const auto nano_seconds = p.value(NANOSECOND);

    integer second = nano_seconds / FACTOR_SECOND;

    integer minute = second / (FACTOR_MINUTE / FACTOR_SECOND);
    second %= (FACTOR_MINUTE / FACTOR_SECOND);

    integer hour = minute / (FACTOR_HOUR / FACTOR_MINUTE);
    minute %= (FACTOR_HOUR / FACTOR_MINUTE);

    integer day = hour / (FACTOR_DAY / FACTOR_HOUR);
    hour %= (FACTOR_DAY / FACTOR_HOUR);

    integer year = EPOCH_YEAR;
    while (true){
        const integer days_in_year = isLeapYear(year) ? DAYS_LEAP_YEAR : DAYS_COMMON_YEAR;
        if (day < days_in_year){
            break;
        }

        day -= days_in_year;
        year += 1;
    }

    integer month = 1;
    while (true){
        const integer dim = daysOfMonth(year, month);
        if (day < dim){
            break;
        }

        day -= dim;
        month += 1;
    }

    day += 1;

    this->set(year, month, day, hour, minute, second);
}

inline original::time::UTCTime::UTCTime(UTCTime&& other) noexcept : UTCTime() {
    this->operator=(std::move(other));
}

inline original::time::UTCTime&
original::time::UTCTime::operator=(UTCTime&& other) noexcept {
    if (this == &other)
        return *this;

    this->set(other.year_, other.month_, other.day_,
              other.hour_, other.minute_, other.second_);
    other.set(EPOCH_YEAR, EPOCH_MONTH, EPOCH_DAY, 0, 0, 0);
    return *this;
}

inline original::integer
original::time::UTCTime::value(const unit unit) const noexcept {
    switch (unit) {
        case SECOND:
            return this->second_;
        case MINUTE:
            return this->minute_;
        case HOUR:
            return this->hour_;
        case DAY:
            return this->day_;
        default:
            break;
    }
    return 0;
}

inline original::integer
original::time::UTCTime::value(const calendar calendar) const noexcept {
    switch (calendar) {
        case MONTH:
            return this->month_;
        case YEAR:
        default:
            return this->year_;
    }
}

inline original::time::UTCTime::operator point() const {
    time_val_type total_days = 0;

    for (integer y = EPOCH_YEAR; y < this->year_; ++y) {
        total_days += isLeapYear(y) ? DAYS_LEAP_YEAR : DAYS_COMMON_YEAR;
    }

    for (integer m = 1; m < this->month_; ++m) {
        total_days += daysOfMonth(this->year_, m);
    }

    total_days += this->day_ - 1;

    time_val_type total_seconds = total_days * (FACTOR_DAY / FACTOR_SECOND);
    total_seconds += this->hour_ * (FACTOR_HOUR / FACTOR_SECOND);
    total_seconds += this->minute_ * (FACTOR_MINUTE / FACTOR_SECOND);
    total_seconds += this->second_;

    return point{total_seconds, SECOND};
}

inline original::integer
original::time::UTCTime::compareTo(const UTCTime& other) const {
    if (this->year_ != other.year_)
        return this->year_ > other.year_ ? 1 : -1;
    if (this->month_ != other.month_)
        return this->month_ > other.month_ ? 1 : -1;
    if (this->day_ != other.day_)
        return this->day_ > other.day_ ? 1 : -1;
    if (this->hour_ != other.hour_)
        return this->hour_ > other.hour_ ? 1 : -1;
    if (this->minute_ != other.minute_)
        return this->minute_ > other.minute_ ? 1 : -1;
    if (this->second_ != other.second_)
        return this->second_ > other.second_ ? 1 : -1;
    return 0;
}

inline original::u_integer
original::time::UTCTime::toHash() const noexcept {
    u_integer seed = 0;
    hash<UTCTime>::hashCombine(seed, this->year_, this->month_, this->day_,
                               this-> hour_, this->minute_, this->second_);
    return seed;
}

inline std::string original::time::UTCTime::className() const {
    return "time::UTCTime";
}

inline std::string original::time::UTCTime::toString(const bool enter) const {
    std::stringstream ss;
    ss << "(" << this->className() << " "
       << this->year_ << "-"
       << std::setw(2) << std::setfill('0') << this->month_ << "-"
       << std::setw(2) << std::setfill('0') << this->day_ << " "
       << std::setw(2) << std::setfill('0') << this->hour_ << ":"
       << std::setw(2) << std::setfill('0') << this->minute_ << ":"
       << std::setw(2) << std::setfill('0') << this->second_ << ")";
    if (enter)
        ss << "\n";
    return ss.str();
}

inline original::time::UTCTime
original::operator-(const time::UTCTime& t, const time::duration& d) {
    return time::UTCTime{static_cast<time::point>(t) - d};
}

inline original::time::duration
original::operator-(const time::UTCTime& lhs, const time::UTCTime& rhs) {
    return static_cast<time::point>(lhs) - static_cast<time::point>(rhs);
}

inline original::time::UTCTime
original::operator+(const time::UTCTime& t, const time::duration& d) {
    return time::UTCTime{static_cast<time::point>(t) + d};
}

#endif //ORIGINAL_ZEIT_H
