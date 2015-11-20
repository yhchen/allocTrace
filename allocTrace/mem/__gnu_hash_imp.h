#ifndef __SG2D__GNU_HASH_IMP_H__
#define __SG2D__GNU_HASH_IMP_H__

#ifndef _MSC_VER
namespace std 
{
    namespace tr1
    {
		using namespace SG2D;
#endif
        template <>
        struct std::hash<SG2D::AnsiString>
        : public std::unary_function<SG2D::AnsiString, std::size_t>
        {      
            std::size_t
            operator()(const SG2D::AnsiString& __s) const
            { return SG2D::hash_elements(__s.ptr(), __s.length()); }
        };
        template <>
		struct std::hash<SG2D::UnicodeStringBase>
        : public std::unary_function<SG2D::UnicodeStringBase, std::size_t>
        {      
            std::size_t
            operator()(const SG2D::UnicodeStringBase& __s) const
            { return SG2D::hash_elements(__s.ptr(), __s.length()); }
        };

#ifndef _MSC_VER
    }
}
#endif


#endif
