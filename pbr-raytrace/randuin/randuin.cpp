#include <main>

namespace core {
    core::buffer<vec4s> RanduinWrynn::deck;
    int RanduinWrynn::turn = 0;

    void RanduinWrynn::construct(const int& cards) {
        if (cards < 1)
            return;
        core::Timer<float> timer;
        timer.start();
        turn = 0;
        deck.reserve(cards);
        deck.count() = cards;
        srand(timer.now());
        for (auto& i:deck)
            i = (vec4s((float)(rand() % 1000 - 500), (float)(rand() % 1000 - 500), (float)(rand() % 1000 - 500), 500.0f) / 500.0f).normalized3d();
    }

   void RanduinWrynn::mulligan() {
        core::Timer<float> timer;
        srand(rand());
        for (auto& i:deck)
            i = (vec4s((float)(rand() % 1000 - 500), (float)(rand() % 1000 - 500), (float)(rand() % 1000 - 500), 500.0f) / 500.0f).normalized3d();
    }

    const vec4s RanduinWrynn::topdeck() {
        const int tmp = turn++;
        turn %= deck.size();
        return deck[tmp];
    }

}

