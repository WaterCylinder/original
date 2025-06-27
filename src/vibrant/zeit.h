#ifndef ORIGINAL_ZEIT_H
#define ORIGINAL_ZEIT_H
#include <cmath>
#include "config.h"
#include "comparable.h"
#include "hash.h"
#include "printable.h"


namespace original {
    class time final {
    public:
        using time_val_type = integer;
    private:
        static constexpr time_val_type UNIT_FACTOR_BASE = 1;

        static inline constexpr time_val_type UNIT_FACTOR[] {
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

        static constexpr unit NANOSECOND = unit::NANOSECOND;
        static constexpr unit MICROSECOND = unit::MICROSECOND;
        static constexpr unit MILLISECOND = unit::MILLISECOND;
        static constexpr unit SECOND = unit::SECOND;
        static constexpr unit MINUTE = unit::MINUTE;
        static constexpr unit HOUR = unit::HOUR;
        static constexpr unit DAY = unit::DAY;


        static constexpr time_val_type FACTOR_NANOSECOND = UNIT_FACTOR[static_cast<ul_integer>(NANOSECOND)];
        static constexpr time_val_type FACTOR_MICROSECOND = UNIT_FACTOR[static_cast<ul_integer>(MICROSECOND)];
        static constexpr time_val_type FACTOR_MILLISECOND = UNIT_FACTOR[static_cast<ul_integer>(MILLISECOND)];
        static constexpr time_val_type FACTOR_SECOND = UNIT_FACTOR[static_cast<ul_integer>(SECOND)];
        static constexpr time_val_type FACTOR_MINUTE = UNIT_FACTOR[static_cast<ul_integer>(MINUTE)];
        static constexpr time_val_type FACTOR_HOUR = UNIT_FACTOR[static_cast<ul_integer>(HOUR)];
        static constexpr time_val_type FACTOR_DAY = UNIT_FACTOR[static_cast<ul_integer>(DAY)];

        class duration : public comparable<duration>, public hashable<duration> {
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
            static point now();

            explicit point(time_val_type val = 0, unit unit = MILLISECOND);

            explicit point(duration  d);

            [[nodiscard]] time_val_type value(unit unit = MILLISECOND) const noexcept;

            [[nodiscard]] integer compareTo(const point& other) const override;

            u_integer toHash() const noexcept override;

            std::string toString(bool enter) const override;

            point& operator+=(const duration& d);

            point& operator-=(const duration& d);

            friend point operator+(const point& p, const duration& d);

            friend point operator-(const point& p, const duration& d);

            friend duration operator-(const point& lhs, const point& rhs);
        };
    };

    const time::duration original::time::duration::ZERO = duration{};

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

    namespace literals {
        inline time::duration operator"" _ns(unsigned long long val) {
            return time::duration(static_cast<time::time_val_type>(val), time::NANOSECOND);
        }

        inline time::duration operator"" _us(unsigned long long val) {
            return time::duration(static_cast<time::time_val_type>(val), time::MICROSECOND);
        }

        inline time::duration operator"" _ms(unsigned long long val) {
            return time::duration(static_cast<time::time_val_type>(val), time::MILLISECOND);
        }

        inline time::duration operator"" _s(unsigned long long val) {
            return time::duration(static_cast<time::time_val_type>(val), time::SECOND);
        }

        inline time::duration operator"" _min(unsigned long long val) {
            return time::duration(static_cast<time::time_val_type>(val), time::MINUTE);
        }

        inline time::duration operator"" _h(unsigned long long val) {
            return time::duration(static_cast<time::time_val_type>(val), time::HOUR);
        }

        inline time::duration operator""_d(unsigned long long val) {
            return time::duration(static_cast<time::time_val_type>(val), time::DAY);
        }

        inline time::duration operator"" _ns(long double val) {
            return time::duration(static_cast<time::time_val_type>(val * time::FACTOR_NANOSECOND + 0.5), time::NANOSECOND);
        }

        inline time::duration operator"" _us(long double val) {
            return time::duration(static_cast<time::time_val_type>(val * time::FACTOR_MICROSECOND + 0.5), time::NANOSECOND);
        }

        inline time::duration operator"" _ms(long double val) {
            return time::duration(static_cast<time::time_val_type>(val * time::FACTOR_MILLISECOND + 0.5), time::NANOSECOND);
        }

        inline time::duration operator"" _s(long double val) {
            return time::duration(static_cast<time::time_val_type>(val * time::FACTOR_SECOND + 0.5), time::NANOSECOND);
        }

        inline time::duration operator"" _min(long double val) {
            return time::duration(static_cast<time::time_val_type>(val * time::FACTOR_MINUTE + 0.5), time::NANOSECOND);
        }

        inline time::duration operator"" _h(long double val) {
            return time::duration(static_cast<time::time_val_type>(val * time::FACTOR_HOUR + 0.5), time::NANOSECOND);
        }

        inline time::duration operator""_d(long double val) {
            return time::duration(static_cast<time::time_val_type>(std::llround(val * time::FACTOR_DAY)), time::NANOSECOND);
        }
    }
}

original::time::duration::duration(time_val_type val, unit unit) {
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

original::time::duration::duration(duration&& other) noexcept : duration() {
    this->operator=(std::move(other));
}

original::time::duration&
original::time::duration::operator=(duration&& other) noexcept {
    if (this == &other)
        return *this;

    this->nano_seconds_ = other.nano_seconds_;
    other.nano_seconds_ = 0;
    return *this;
}

original::time::time_val_type
original::time::duration::value(unit unit) const noexcept {
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

original::integer
original::time::duration::compareTo(const duration& other) const {
    if (this->nano_seconds_ == other.nano_seconds_)
        return 0;
    return this->nano_seconds_ > other.nano_seconds_ ? 1 : -1;
}

original::time::duration&
original::time::duration::operator+=(const duration& other) {
    this->nano_seconds_ += other.nano_seconds_;
    return *this;
}

original::time::duration&
original::time::duration::operator-=(const duration& other) {
    this->nano_seconds_ -= other.nano_seconds_;
    return *this;
}

original::time::duration&
original::time::duration::operator*=(time_val_type factor) {
    this->nano_seconds_ *= factor;
    return *this;
}

original::time::duration&
original::time::duration::operator/=(time_val_type factor) {
    this->nano_seconds_ /= factor;
    return *this;
}

original::time::duration&
original::time::duration::operator/=(const time::duration& other) {
    this->nano_seconds_ /= other.nano_seconds_;
    return *this;
}

original::floating
original::time::duration::div(time_val_type factor, unit unit) const {
    return this->div(duration{factor, unit});
}

original::floating
original::time::duration::div(const time::duration& other) const {
    return static_cast<floating>(this->nano_seconds_) / static_cast<floating>(other.nano_seconds_);
}

original::time::duration
original::operator-(const time::duration& d) {
    time::duration res(d);
    res.nano_seconds_ = -res.nano_seconds_;
    return res;
}

original::time::duration
original::operator+(const time::duration& lhs, const time::duration& rhs) {
    time::duration d(lhs);
    d += rhs;
    return d;
}

original::time::duration
original::operator-(const time::duration& lhs, const time::duration& rhs) {
    time::duration d(lhs);
    d -= rhs;
    return d;
}

original::time::duration
original::operator*(const time::duration &d, time::time_val_type factor) {
    time::duration res(d);
    res *= factor;
    return res;
}

original::time::duration
original::operator*(time::time_val_type factor, const time::duration& d) {
    time::duration res(d);
    res *= factor;
    return res;
}

original::time::duration
original::operator/(const time::duration &d, time::time_val_type factor) {
    time::duration res(d);
    res /= factor;
    return res;
}

original::time::duration
original::operator/(const time::duration& lhs, const time::duration& rhs) {
    time::duration d(lhs);
    d /= rhs;
    return d;
}

original::time::duration
original::abs(const time::duration& d) {
    time::time_val_type val = d.nano_seconds_;
    val = val < 0 ? -val : val;
    return time::duration{val, time::NANOSECOND};
}

original::time::point original::time::point::now() {
#if ORIGINAL_COMPILER_GCC || (ORIGINAL_COMPILER_CLANG && ORIGINAL_PLATFORM_LINUX)
    struct timespec ts{};
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

original::time::point::point(time::time_val_type val, time::unit unit)
    : nano_since_epoch_(val, unit) {}

original::time::point::point(time::duration d)
    : nano_since_epoch_(std::move(d)) {}

original::time::time_val_type
original::time::point::value(time::unit unit) const noexcept {
    return this->nano_since_epoch_.value(unit);
}

original::integer
original::time::point::compareTo(const time::point& other) const {
    return this->nano_since_epoch_.compareTo(other.nano_since_epoch_);
}

original::u_integer
original::time::point::toHash() const noexcept {
    return this->nano_since_epoch_.toHash();
}

std::string
original::time::point::toString(bool enter) const {
    std::stringstream ss;
    ss << "(point " << this->nano_since_epoch_.nano_seconds_ << ")";
    if (enter)
        ss << "\n";
    return ss.str();
}

original::time::point&
original::time::point::operator+=(const time::duration& d) {
    this->nano_since_epoch_ += d;
    return *this;
}

original::time::point&
original::time::point::operator-=(const time::duration& d) {
    this->nano_since_epoch_ -= d;
    return *this;
}

original::time::point
original::operator-(const time::point &p, const time::duration &d) {
    time::point res(p);
    res -= d;
    return res;
}

original::time::duration
original::operator-(const time::point &lhs, const time::point &rhs) {
    return lhs.nano_since_epoch_ - rhs.nano_since_epoch_;
}

original::time::point
original::operator+(const time::point &p, const time::duration &d) {
    time::point res(p);
    res += d;
    return res;
}

#endif //ORIGINAL_ZEIT_H
