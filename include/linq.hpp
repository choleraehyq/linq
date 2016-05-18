#pragma once

#include <algorithm>
#include <cstddef>
#include <functional>
#include <iterator>
#include <map>
#include <type_traits>
#include <vector>
#include <utility>

#include <boost/range.hpp>
#include <boost/range/adaptors.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/range/join.hpp>

namespace linq {
    
    namespace detail {
        
        template <typename Function> 
        struct function_traits: public function_traits<decltype(&Function::operator())>
        {};
        
        template <typename ClassType, typename ReturnType, typename... Args>
        struct function_traits<ReturnType(ClassType::*)(Args...) const> {
            typedef std::function<ReturnType(Args...)> function;  
        };
        
        template <typename Function>
        typename function_traits<Function>::function
        to_function(Function &lambda) {
            return static_cast<typename function_traits<Function>::function>(lambda);
        }
    
        template <typename R>
        class LinqObj {
        private:
            R __range;
            
        public:
            typedef typename R::value_type value_type;
        
        public:
            LinqObj(R &&range): __range(range) {}
        
            auto begin() const {
                return std::begin(__range);
            }
        
            auto end() const {
                return std::end(__range);
            }
        
            template <typename F>
            auto select(const F &f) {
                auto func = to_function(f);
                return LinqObj<
                    boost::range_detail::transformed_range<
                        typename function_traits<F>::function, R>
                    >(boost::adaptors::transform(__range, func));
            }
        
            template <typename F>
            auto where(const F &f) {
                auto func = to_function(f);
                return LinqObj<
                    boost::range_detail::filtered_range<
                        typename function_traits<F>::function, R>
                    >(boost::adaptors::filter(__range, func));
            }
        
            template <typename F>
            value_type max(const F &f) const {
                return *std::max_element(begin(), end(), f);
            }
        
            value_type max() const {
                return *std::max_element(begin(), end());
            }
            
            template <typename F>
            value_type min(const F &f) const {
                return *std::min_element(begin(), end(), f);
            } 
            
            value_type min() const {
                return *std::min_element(begin(), end());
            }
            
            auto reverse() {
                return LinqObj(boost::adaptors::reverse(__range));    
            }
            
            template <typename F>
            auto first(const F &f) {
                return std::find_if(begin(), end(), f);
            }
            
            template <typename F>
            auto last() {
                return reverse().first();
            }
            
            bool empty() {
                return begin() == end();
            }
            
            
            template <typename F>
            bool any(const F &f) const {
                return std::any_of(begin(), end(), f);
            }
            
            template <typename F>
            bool all(const F &f) const {
                return std::all_of(begin(), end(), f);
            }
            
            template <typename F>
            void for_each(const F &f) const {
                std::for_each(begin(), end(), f);
            }
            
            template <typename F>
            bool contains(const F &f) const {
                return std::find_if(begin(), end(), f);
            }
            
            template <typename F>
            auto distinct(const F& f) {
                return LinqObj(boost::range::unique(__range, f));
            }
            
            auto distinct() {
                return LinqObj(boost::uniqued_range<R>(__range | boost::adaptors::uniqued));
            }
            
            template <typename F>
            value_type aggregate(const F &f) const {
                auto init{ begin() };
                auto value = *init;
                return std::accumulate(++init, end(), std::move(value), f);
            }
            
            value_type sum() const {
                return aggregate(std::plus<value_type>());
            }
            
            template <typename F>
            std::ptrdiff_t count(const F &f) {
                return std::count_if(begin, end(), f);    
            }
            
           
            std::ptrdiff_t count() const {
                return std::distance(begin(), end());
            }
            
            value_type average() const {
                return sum() / count();
            }
            
            template <typename R2>
            auto concat(const R2 &other) {
                return LinqObj(boost::join(__range, other));
            }
            
            template <typename T>
            value_type element_at(T index) const {
                return *std::next(begin(), index);
            }
            
            template <typename R2>
            void except(const R2 &other, std::vector<value_type> result) {
                std::set_difference(begin(), end(), std::begin(other), std::end(other),
                                    std::inserter(result, result.begin()));
            }
            
            auto find(const value_type &x) {
                return std::find(begin(), end(), x);
            }
            
            template <typename Fn>
            auto group_by(const Fn& f) {
                using key_t = typename std::result_of<Fn(value_type)>::type;
                std::multimap<key_t, value_type> retval;
                std::for_each(begin(), end(), [&](value_type item) {
                    retval.insert(std::make_pair(f(item), item));    
                });
                return retval;
            }
            
            template <typename KeyFn, typename ValFn>
            auto group_by(const KeyFn &kfn, const ValFn &vfn) {
                using key_t = typename std::result_of<KeyFn(value_type)>::type;
                using val_t = typename std::result_of<ValFn(value_type)>::type;
                std::multimap<key_t, val_t> retval;
                std::for_each(begin(), end(), [&](value_type item) {
                    retval.insert(std::make_pair(kfn(item), vfn(item)));
                });
                return retval;
            }
            
            auto keys() const {
                return LinqObj(boost::adaptors::keys(__range));
            }
            
            auto values() const {
                return LinqObj(boost::adaptors::values(__range));
            }
            
            
            template <typename P>
            auto order_by(const P &pfn) {
                return LinqObj(boost::range::sort(__range, pfn));
            }
            
            auto order_by() {
                return LinqObj(boost::range::sort(__range));
            }
            
            auto order_by_descending() {
                return order_by().reverse();
            }
            
            template <typename T>
            auto skip(T n) const {
                return LinqObj(boost::make_iterator_range(begin()+n, end()));
            }
            
            template <typename F>
            auto skip_while(const F &f) const {
                return LinqObj(boost::make_iterator_range(std::find_if_not(begin(), end(), f), end()));
            }
            
            template <typename R2>
            bool sequence_equal(R2 other) const {
                return count() == std::distance(std::begin(other), std::end(other)) && 
                        std::equal(begin(), end(), std::begin(other), std::end(other));
            }
            
            template <typename T> 
            auto take(T start, T end) const {
                return LinqObj(boost::adaptors::slice(__range, start, end));
            }
            
            template <typename F>
            auto take_while(const F &f) const {
                return LinqObj(boost::make_iterator_range(begin(), std::find_if(begin(), end(), f)));
            }
            
            template <typename R2>
            bool includes(const R2 &other) const {
                return std::includes(begin(), end(), std::begin(other), std::end(other));
            }
            
        
        };
    
        template <
            template <typename T>
            class IteratorRange, 
            typename C>
        using Range = IteratorRange<decltype(std::begin(std::declval<C>()))>;
    
        template <typename ContainerT>
        using iteratorRange = Range<boost::iterator_range, ContainerT>;
    };
    
    template <typename ContainerT>
    auto from(const ContainerT &range) {
        return detail::LinqObj<detail::iteratorRange<ContainerT>>(detail::iteratorRange<ContainerT>(range));
    }
    
};
