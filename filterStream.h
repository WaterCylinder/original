#ifndef FILTERSTREAM_H
#define FILTERSTREAM_H

#include "filter.h"

namespace original{
    template<typename TYPE>
    class filterStream
    {
        enum class opts{AND = 1, OR = 0, NOT = 2, LEFT_BRACKET = 3, RIGHT_BRACKET = 4};

        mutable chain<std::shared_ptr<filter<TYPE>>> stream;
        mutable chain<opts> ops;
        mutable bool flag;

        protected:
            explicit filterStream();
            void addBrackets();
            void addAndOpt();
            void addOrOpt();
            void addNotOpt();
            void pushEnd(const filter<TYPE>& f);
            void pushAll(const filterStream& fs);
            void toPostFix() const;
        public:
            ~filterStream() = default;
            filterStream& operator&&(const filter<TYPE>& f);
            filterStream& operator&&(const filterStream& fs);
            filterStream& operator||(const filter<TYPE>& f);
            filterStream& operator||(const filterStream& fs);
            filterStream& operator!();
            bool operator()(const TYPE& t) const;

            template<typename T>
            friend filterStream<T> operator&&(const filter<T>& f1, const filter<T>& f2);
            template<typename T>
            friend filterStream<T> operator&&(const filter<T>& f, const filterStream<T>& ofs);
            template<typename T>
            friend filterStream<T> operator||(const filter<T>& f1, const filter<T>& f2);
            template<typename T>
            friend filterStream<T> operator||(const filter<T>& f, const filterStream<T>& ofs);
            template<typename T>
            friend filterStream<T> operator!(const filter<T>& f);
            template<typename T>
            friend filterStream<T> group(const filterStream<T>& ofs);
            template<typename T>
            friend filterStream<T> group(const filter<T>& f);
    };

    template<typename T>
    filterStream<T> operator&&(const filter<T>& f1, const filter<T>& f2);
    template<typename T>
    filterStream<T> operator&&(const filter<T>& f, const filterStream<T>& ofs);
    template<typename T>
    filterStream<T> operator||(const filter<T>& f1, const filter<T>& f2);
    template<typename T>
    filterStream<T> operator||(const filter<T>& f, const filterStream<T>& ofs);
    template<typename T>
    filterStream<T> operator!(const filter<T>& f);
    template<typename T>
    filterStream<T> group(const filterStream<T>& ofs);
    template<typename T>
    filterStream<T> group(const filter<T>& f);
} // namespace original

    template <typename TYPE>
    original::filterStream<TYPE>::filterStream() : stream(), ops(), flag(false) {}

    template <typename TYPE>
    auto original::filterStream<TYPE>::addBrackets() -> void
    {
        this->stream.pushBegin(nullptr);
        this->stream.pushEnd(nullptr);
        this->ops.pushBegin(opts::LEFT_BRACKET);
        this->ops.pushEnd(opts::RIGHT_BRACKET);
    }

    template <typename TYPE>
    auto original::filterStream<TYPE>::addAndOpt() -> void
    {
        this->stream.pushEnd(nullptr);
        this->ops.pushEnd(opts::AND);
    }

    template <typename TYPE>
    auto original::filterStream<TYPE>::addOrOpt() -> void
    {
        this->stream.pushEnd(nullptr);
        this->ops.pushEnd(opts::OR);
    }

    template <typename TYPE>
    auto original::filterStream<TYPE>::addNotOpt() -> void
    {
        this->stream.pushBegin(nullptr);
        this->ops.pushBegin(opts::NOT);
    }

    template <typename TYPE>
    auto original::filterStream<TYPE>::pushEnd(const filter<TYPE>& f) -> void
    {
        this->stream.pushEnd(std::shared_ptr<filter<TYPE>>(f.clone()));
    }

    template <typename TYPE>
    auto original::filterStream<TYPE>::pushAll(const filterStream& fs) -> void
    {
        for (const auto& filter: fs.stream)
        {
            this->stream.pushEnd(filter);
        }
        for (const auto& op: fs.ops)
        {
            this->ops.pushEnd(op);
        }
    }

    template <typename TYPE>
    auto original::filterStream<TYPE>::toPostFix() const -> void{
        this->flag = true;

        chain<std::shared_ptr<filter<TYPE>>> stream_post;
        chain<opts> ops_post;
        chain<opts> ops_tmp;

        auto it_stream = this->stream.begins();
        auto it_ops = this->ops.begins();

        while (it_stream->isValid()){
            if (it_stream->get() != nullptr){
                stream_post.pushEnd(it_stream->get());
            } else if (it_ops->isValid()){
                switch (it_ops->get()) {
                    case opts::LEFT_BRACKET:
                        ops_tmp.pushEnd(opts::LEFT_BRACKET);
                        break;
                    case opts::RIGHT_BRACKET:
                        while (!ops_tmp.empty() && ops_tmp[-1] != opts::LEFT_BRACKET){
                            stream_post.pushEnd(nullptr);
                            ops_post.pushEnd(ops_tmp.popEnd());
                        }
                        ops_tmp.popEnd();
                        break;
                    case opts::NOT:
                        ops_tmp.pushEnd(opts::NOT);
                        break;
                    default:
                        while (!ops_tmp.empty()
                               && ops_tmp[-1] >= it_ops->get()
                               && ops_tmp[-1] != opts::LEFT_BRACKET){
                            stream_post.pushEnd(nullptr);
                            ops_post.pushEnd(ops_tmp.popEnd());
                        }
                        ops_tmp.pushEnd(it_ops->get());
                        break;
                }
                it_ops->next();
            }
            it_stream->next();
        }

        while (!ops_tmp.empty()){
            stream_post.pushEnd(nullptr);
            ops_post.pushEnd(ops_tmp.popEnd());
        }
        this->stream = stream_post;
        this->ops = ops_post;
    }

    template <typename TYPE>
    auto original::filterStream<TYPE>::operator&&(const filter<TYPE>& f) -> filterStream&
    {
        this->addAndOpt();
        this->pushEnd(f);
        return *this;
    }

    template <typename TYPE>
    auto original::filterStream<TYPE>::operator&&(const filterStream& fs) -> filterStream&
    {
        this->addAndOpt();
        this->pushAll(fs);
        return *this;
    }

    template <typename TYPE>
    auto original::filterStream<TYPE>::operator||(const filter<TYPE>& f) -> filterStream&
    {
        this->addOrOpt();
        this->pushEnd(f);
        return *this;
    }

    template <typename TYPE>
    auto original::filterStream<TYPE>::operator||(const filterStream& fs) -> filterStream&
    {
        this->addOrOpt();
        this->pushAll(fs);
        return *this;
    }

    template <typename TYPE>
    auto original::filterStream<TYPE>::operator!() -> filterStream&
    {
        this->addBrackets();
        this->addNotOpt();
        return *this;
    }

    template <typename TYPE>
    original::filterStream<TYPE> original::operator&&(const filter<TYPE>& f1, const filter<TYPE>& f2)
    {
        filterStream<TYPE> fs;
        fs.pushEnd(f1);
        fs.addAndOpt();
        fs.pushEnd(f2);
        return fs;
    }

    template <typename TYPE>
    original::filterStream<TYPE> original::operator&&(const filter<TYPE>& f, const filterStream<TYPE>& ofs)
    {
        filterStream<TYPE> fs;
        fs.pushEnd(f);
        fs.addAndOpt();
        fs.pushAll(ofs);
        return fs;
    }

    template <typename TYPE>
    original::filterStream<TYPE> original::operator||(const filter<TYPE>& f1, const filter<TYPE>& f2)
    {
        filterStream<TYPE> fs;
        fs.pushEnd(f1);
        fs.addOrOpt();
        fs.pushEnd(f2);
        return fs;
    }

    template <typename TYPE>
    original::filterStream<TYPE> original::operator||(const filter<TYPE>& f, const filterStream<TYPE>& ofs)
    {
        filterStream<TYPE> fs;
        fs.pushEnd(f);
        fs.addOrOpt();
        fs.pushAll(ofs);
        return fs;
    }

    template <typename TYPE>
    original::filterStream<TYPE> original::operator!(const filter<TYPE>& f)
    {
        filterStream<TYPE> fs;
        fs.pushEnd(f);
        fs.addNotOpt();
        return fs;
    }

    template<typename TYPE>
    original::filterStream<TYPE> original::group(const filterStream<TYPE> &ofs) {
        filterStream<TYPE> fs;
        fs.pushAll(ofs);
        fs.addBrackets();
        return fs;
    }

    template<typename TYPE>
    original::filterStream<TYPE> original::group(const filter<TYPE> &f) {
        filterStream<TYPE> fs;
        fs.pushEnd(f);
        return fs;
    }

    template <typename TYPE>
    bool original::filterStream<TYPE>::operator()(const TYPE& t) const
    {
        if (!this->flag) this->toPostFix();

        chain<bool> value_stack;
        auto it_stream = this->stream.begins();
        auto it_ops = this->ops.begins();

        while (it_stream->isValid()){
            if (it_stream->get() != nullptr){
                value_stack.pushEnd((*it_stream->get())(t));
            } else if (it_ops->isValid()){
                switch (it_ops->get()) {
                    case opts::NOT:
                        value_stack[-1] = !value_stack[-1];
                        break;
                    default:
                        const bool right = value_stack.popEnd();
                        const bool left = value_stack.popEnd();
                        value_stack.pushEnd(it_ops->get() == opts::AND ?
                                            left && right : left || right);
                        break;
                }
                it_ops->next();
            }
            it_stream->next();
        }
        return value_stack[-1];
    }

#endif //FILTERSTREAM_H
