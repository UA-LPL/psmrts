#ifndef VariantTraits_hpp
#define VariantTraits_hpp

#include <string>
#include <tuple>
#include <utility>
#include <type_traits>

#include <psmrts/core/PsmrtsUtilities.hpp>


namespace psmrts::algorithm::variants { 

  /**
   * @brief Various variant traits that will iterate through all types
   * 
   * These sets of traits, pulled from Stackoverflow, provides iterations
   * through all std::variant types at compile time. This is useful to
   * automatically register all variants within a class that has common API
   * methods such as registrations.
   * 
   * See PsmrtsTracer and PsmrtsShape for usage.
   * 
   * Sources are credited as shown.
   */

    // Source - https://stackoverflow.com/a/57642181
    // Posted by Yakk - Adam Nevraumont, modified by community. See post 'Timeline' for change history
    // Retrieved 2026-02-05, License - CC BY-SA 4.0

      template<std::size_t I>
      using index_t = std::integral_constant<std::size_t, I>;
      template<std::size_t I>
      constexpr index_t<I> index{};

      template<std::size_t...Is>
      constexpr std::tuple< index_t<Is>... > make_indexes(std::index_sequence<Is...>){
        return std::make_tuple(index<Is>...);
      }
      template<std::size_t N>
      constexpr auto indexing_tuple = make_indexes(std::make_index_sequence<N>{});

    // Source - https://stackoverflow.com/a/57642181
    // Posted by Yakk - Adam Nevraumont, modified by community. See post 'Timeline' for change history
    // Retrieved 2026-02-05, License - CC BY-SA 4.0

    template<std::size_t...Is, class T, class F>
    auto tuple_foreach( std::index_sequence<Is...>, T&& tup, F&& f ) {
      ( f( std::get<Is>( std::forward<T>(tup) ) ), ... );
    }
    template<class T, class F>
    auto tuple_foreach( T&& tup, F&& f ) {
      auto indexes = std::make_index_sequence< std::tuple_size_v< std::decay_t<T> > >{};
      return tuple_foreach( indexes, std::forward<T>(tup), std::forward<F>(f) );
    }


} // namespace psmrts::algorithms::variants

#endif

