#ifndef __KERNELFUNCTION_H__
#define __KERNELFUNCTION_H__

namespace bicycle
{
    template<typename Result,typename ...Args>
    struct abstract_function
    {
        virtual Result operator()(Args... args) =0;
        virtual abstract_function *clone() const =0;
        virtual ~abstract_function() {};
    };
    
    template<typename Func,typename Result,typename ...Args>
    class concrete_function: public abstract_function<Result,Args...>
    {
        Func f;
    public:
        concrete_function(const Func &x)
        : f(x)
        {}
        Result operator()(Args... args) override
        {
            return f(args...);
        }
        concrete_function *clone() const override
        {
            return new concrete_function{f};
        }
    };

    template<typename Func>
    struct func_filter
    {
        typedef Func type;
    };
    template<typename Result,typename ...Args>
    struct func_filter<Result(Args...)>
    {
        typedef Result (*type)(Args...);
    };

    template<typename signature>
    class function;

    template<typename Result,typename ...Args>
    class function<Result(Args...)>
    {
        abstract_function<Result,Args...> *f;
    public:
        function()
        : f(nullptr)
        {}
        template<typename Func> function(const Func &x)
        : f(new concrete_function<typename func_filter<Func>::type,Result,Args...>(x))
        {}
        function(const function &rhs)
        : f(rhs.f ? rhs.f->clone() : nullptr)
        {}
        function &operator=(const function &rhs)
        {
            if( (&rhs != this ) && (rhs.f) )
            {
                auto *temp = rhs.f->clone();
                delete f;
                f = temp;
            }
            return *this;
        }
        template<typename Func> function &operator=(const Func &x)
        {
            auto *temp = new concrete_function<typename func_filter<Func>::type,Result,Args...>(x);
            delete f;
            f = temp;
            return *this;
        }
        Result operator()(Args... args) const
        {
            if(f)
                return (*f)(args...);
            else
                return Result{};
        }
        ~function()
        {
            delete f;
        }
    };

    template<typename ...Args>
    class function<void(Args...)>
    {
        abstract_function<void,Args...> *f;
    public:
        function()
        : f(nullptr)
        {}
        template<typename Func> function(const Func &x)
        : f(new concrete_function<typename func_filter<Func>::type,void,Args...>(x))
        {}
        function(const function &rhs)
        : f(rhs.f ? rhs.f->clone() : nullptr)
        {}
        function &operator=(const function &rhs)
        {
            if( (&rhs != this ) && (rhs.f) )
            {
                auto *temp = rhs.f->clone();
                delete f;
                f = temp;
            }
            return *this;
        }
        template<typename Func> function &operator=(const Func &x)
        {
            auto *temp = new concrete_function<typename func_filter<Func>::type,void,Args...>(x);
            delete f;
            f = temp;
            return *this;
        }
        void operator()(Args... args) const
        {
            if(f)
                (*f)(args...);
        }
        ~function()
        {
            delete f;
        }
    };

}

#endif // __KERNELFUNCTION_H__
