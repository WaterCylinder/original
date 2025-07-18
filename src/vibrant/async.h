#ifndef ORIGINAL_ASYNC_H
#define ORIGINAL_ASYNC_H

// todo
namespace original {
    class async {
    public:
        template<typename P_TYPE, typename Callback>
        class promise;

        template<typename F_TYPE>
        class future {
            template<typename, typename>
            friend class promise;
        };

        template<typename P_TYPE, typename Callback>
        class promise {
            Callback c;

        public:
            explicit promise();

            explicit promise(Callback callback);
        };


    };
}

#endif //ORIGINAL_ASYNC_H
