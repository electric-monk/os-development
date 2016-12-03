#ifndef __FUNCTION_H__
#define __FUNCTION_H__

namespace Library {
    
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
    
    template<typename signature> class Function;
    
    template<typename Result,typename ...Args> class Function<Result(Args...)>
    {
        abstract_function<Result,Args...> *f;
    public:
        Function()
        : f(nullptr)
        {}
        template<typename Func> Function(const Func &x)
        : f(new concrete_function<typename func_filter<Func>::type,Result,Args...>(x))
        {}
        Function(const Function &rhs)
        : f(rhs.f ? rhs.f->clone() : nullptr)
        {}
        Function &operator=(const Function &rhs)
        {
            if( (&rhs != this ) && (rhs.f) )
            {
                auto *temp = rhs.f->clone();
                delete f;
                f = temp;
            }
            return *this;
        }
        template<typename Func> Function &operator=(const Func &x)
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
        ~Function()
        {
            delete f;
        }
    };
    
    template<typename ...Args> class Function<void(Args...)>
    {
        abstract_function<void,Args...> *f;
    public:
        Function()
        : f(nullptr)
        {}
        template<typename Func> Function(const Func &x)
        : f(new concrete_function<typename func_filter<Func>::type,void,Args...>(x))
        {}
        Function(const Function &rhs)
        : f(rhs.f ? rhs.f->clone() : nullptr)
        {}
        Function &operator=(const Function &rhs)
        {
            if( (&rhs != this ) && (rhs.f) )
            {
                auto *temp = rhs.f->clone();
                delete f;
                f = temp;
            }
            return *this;
        }
        template<typename Func> Function &operator=(const Func &x)
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
        ~Function()
        {
            delete f;
        }
    };
    
    
}

#endif // __FUNCTION_H__
