#ifndef PAIR_H
#define PAIR_H

#define ANAGAMI_DEBUG

#ifdef ANAGAMI_DEBUG

#include <fmt/format.h>
#include <cstdio>
#endif

#include <iostream>
#include <utility>

namespace kpl {

    template <typename T1, typename T2>
    class Pair
    {
    public:

        /* Main reason for all the complication. Always default constructable despite sub type */
        Pair()
            : mStateInfo { 0b00000000 }
        {  }

        /* Move based constructor */
        Pair(T1&& t1, T2&& t2)
            : mStateInfo { FIRST_CONSTRUCTED | SECOND_CONSTRUCTED }
        {
            new (  reinterpret_cast<void*>( mFirstMemBlock) ) T1( std::move(t1) );
            new (  reinterpret_cast<void*>( mSecondMemBlock) ) T2( std::move(t2) );
            
            #ifdef ANAGAMI_DEBUG
                fmt::print("Move Constructor Invoked for kpl::Pair\n");
                fmt::print("State Info: {:b}\n", mStateInfo);
            #endif
        }

        /* Value based constructor */
        Pair(const T1& t1, const T2& t2)
            : mStateInfo { FIRST_CONSTRUCTED | SECOND_CONSTRUCTED }
        {
            new (  reinterpret_cast<void*>( mFirstMemBlock) ) T1( t1 );
            new (  reinterpret_cast<void*>( mSecondMemBlock) ) T2( t2 );
            
            #ifdef ANAGAMI_DEBUG
                fmt::print("Value Constructor Invoked for kpl::Pair\n");
                fmt::print("State Info: {:b}\n", mStateInfo);
            #endif
        }
        
        bool isT1Set()
        {
            return (mStateInfo & FIRST_CONSTRUCTED);
        }
        
        bool isT2Set()
        {
            return (mStateInfo & SECOND_CONSTRUCTED);
        }

        void setT1WithCopy(const T1& t1)
        {
            mStateInfo |= FIRST_CONSTRUCTED;
            new (  reinterpret_cast<void*>( mFirstMemBlock) ) T1( t1 );
        }

        void setT2WithCopy(const T2& t2)
        {
            mStateInfo |= SECOND_CONSTRUCTED;
            new (  reinterpret_cast<void*>( mSecondMemBlock) ) T2( t2 );
        }

        void setT1WithMove(T1&& t1)
        {
            mStateInfo |= FIRST_CONSTRUCTED;
            new (  reinterpret_cast<void*>( mFirstMemBlock) ) T1( std::move(t1) );
        }

        void setT2WithMove(T2&& t2)
        {
            mStateInfo |= SECOND_CONSTRUCTED;
            new (  reinterpret_cast<void*>( mSecondMemBlock) ) T2( std::move(t2) );
        }

        template <typename... Ts>
        void emplaceT1(Ts&&... args)
        {
            mStateInfo |= FIRST_CONSTRUCTED;
            new (  reinterpret_cast<void*>( mFirstMemBlock ) ) T1( static_cast<Ts&&>(args)... );
        }

        template <typename... Ts>
        void emplaceT2(Ts&&... args)
        {
            mStateInfo |= SECOND_CONSTRUCTED;
            new (  reinterpret_cast<void*>( mSecondMemBlock ) ) T2( static_cast<Ts&&>(args)... );
        }

        /* Getters mFirst */
        const T1 * cPtr1() const
        {
            #ifdef ANAGAMI_DEBUG
                if(! (mStateInfo & FIRST_CONSTRUCTED) )
                {
                    fmt::print(stderr, "Error: Trying to access unconstructed value in kpl::Pair\n" );
                    assert(mStateInfo & FIRST_CONSTRUCTED);
                }
            #endif

            return reinterpret_cast<const T1 *>(mFirstMemBlock);
        }

        T1 * ptr1()
        {
            #ifdef ANAGAMI_DEBUG
                if(! (mStateInfo & FIRST_CONSTRUCTED) )
                {
                    fmt::print(stderr, "Error: Trying to access unconstructed value in kpl::Pair\n" );
                    assert(mStateInfo & FIRST_CONSTRUCTED);
                }
            #endif

            return reinterpret_cast<T1 *>(mFirstMemBlock);
        }

        T1 t1() const
        {
            #ifdef ANAGAMI_DEBUG
                if(! (mStateInfo & FIRST_CONSTRUCTED) )
                {
                    fmt::print(stderr, "Error: Trying to access unconstructed value in kpl::Pair\n" );
                    assert(mStateInfo & FIRST_CONSTRUCTED);
                }
            #endif

            return *(reinterpret_cast<const T1*>(mFirstMemBlock));
        }

        const T1 cT1() const
        {
            #ifdef ANAGAMI_DEBUG
                if(! (mStateInfo & FIRST_CONSTRUCTED) )
                {
                    fmt::print(stderr, "Error: Trying to access unconstructed value in kpl::Pair\n" );
                    assert(mStateInfo & FIRST_CONSTRUCTED);
                }
            #endif

            return *(reinterpret_cast<const T1*>(mFirstMemBlock));
        }

        /* Getters mSecond */
        const T2 * cPtr2() const
        {
            #ifdef ANAGAMI_DEBUG
                if(! (mStateInfo & SECOND_CONSTRUCTED) )
                {
                    fmt::print(stderr, "Error: Trying to access unconstructed value in kpl::Pair\n" );
                    assert(mStateInfo & SECOND_CONSTRUCTED);
                }
            #endif

            return reinterpret_cast<const T2 *>(mSecondMemBlock);
        }

        T2 * ptr2()
        {
            #ifdef ANAGAMI_DEBUG
                if(! (mStateInfo & SECOND_CONSTRUCTED) )
                {
                    fmt::print(stderr, "Error: Trying to access unconstructed value in kpl::Pair\n" );
                    assert(mStateInfo & SECOND_CONSTRUCTED);
                }
            #endif

            return reinterpret_cast<T2 *>(mSecondMemBlock);
        }

        T2 t2() const
        {
            #ifdef ANAGAMI_DEBUG
                if(! (mStateInfo & SECOND_CONSTRUCTED) )
                {
                    fmt::print(stderr, "Error: Trying to access unconstructed value in kpl::Pair\n" );
                    assert(mStateInfo & SECOND_CONSTRUCTED);
                }
            #endif

            return *(reinterpret_cast<const T2*>(mSecondMemBlock));
        }

        const T2 cT2() const
        {
            #ifdef ANAGAMI_DEBUG
                if(! (mStateInfo & SECOND_CONSTRUCTED) )
                {
                    fmt::print(stderr, "Error: Trying to access unconstructed value in kpl::Pair\n" );
                    assert(mStateInfo & SECOND_CONSTRUCTED);
                }
            #endif

            return *(reinterpret_cast<const T2*>(mSecondMemBlock));
        }

    private:

        static constexpr uint8_t FIRST_CONSTRUCTED = 0b10000000;
        static constexpr uint8_t SECOND_CONSTRUCTED = 0b01000000;

        uint8_t mFirstMemBlock[ sizeof(T1) ];
        uint8_t mSecondMemBlock[ sizeof(T2) ];
        uint8_t mStateInfo;
    };
}

#endif // PAIR_H
