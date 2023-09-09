#pragma once

#include <sstream>
#include <cstddef>
#include <type_traits>

namespace muse
{
    namespace impl
    {
        template<class, class = void>
        struct is_writable : std::false_type {};

        template<class T>
        struct is_writable<T, std::void_t<decltype(std::declval<std::ostream&>() << std::declval<T>())>> : std::true_type {};

        template<class T>
        inline constexpr auto is_writable_v = is_writable<T>::value;

        template<class T>
        concept IsWritable = is_writable_v<T>;

        template<IsWritable T>
        void format_helper(const std::string& str, std::stringstream& strm, std::size_t& pos, T&& value)
        {
            const auto old_pos = pos;
            static const std::string format_pattern{"{}"};

            pos = str.find(format_pattern, pos);
            if(pos != std::string::npos)
            {
                strm << str.substr(old_pos, pos - old_pos) << value;
                pos += format_pattern.length();
            }
        }

        template<class Last>
        void format(const std::string& str, std::stringstream& strm, std::size_t& pos, Last&& last)
        {
            format_helper(str, strm, pos, std::forward<Last>(last));

            if(pos != std::string::npos)
            {
                strm << str.substr(pos);
            }
        }

        template<class Last, class... Args>
        void format(const std::string& str, std::stringstream& strm, std::size_t& pos, Last&& last, Args&& ...args)
        {
            format_helper(str, strm, pos, std::forward<Last>(last));
            format(str, strm, pos, std::forward<Args>(args)...);
        }
    }

    template<class... Args>
    std::string format(const std::string& str, Args&& ...args)
    {
        if constexpr(sizeof...(Args) > 0)
        {
            std::stringstream strm{};
            std::size_t pos = 0u;

            impl::format(str, strm, pos, std::forward<Args>(args)...);
        
            return strm.str();
        }
        
        return str;
    }
}