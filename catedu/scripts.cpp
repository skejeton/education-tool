#include "scripts.hpp"
#define DATA_GUARD 0
#define SCRIPT(name__, ev__)                                                   \
    if (strcmp(event.script_name, name__) == 0)                                \
        if (event.ev__)

char *run_script(ScriptData &scr, ScriptEvent event)
{
#pragma region air_cafe_right
    SCRIPT("air_cafe_right", interacted)
    {
        scr.activate_dialog = true;
        scr.dialog = Dialog{"-Chatter-",
                            "Quanto tempo ainda falta?",
                            "How long do we have left?",
                            {{"Next", "air_cafe_right2"}, {"Close", nullptr}}};
    }
    SCRIPT("air_cafe_right2", any)
    {
        scr.activate_dialog = true;
        scr.dialog = Dialog{"-Chatter-",
                            "Quatro horas…",
                            "Four hours…",
                            {{"Next", "air_cafe_right3"}, {"Close", nullptr}}};
    }
    SCRIPT("air_cafe_right3", any)
    {
        scr.activate_dialog = true;
        scr.dialog = Dialog{
            "-Chatter-",
            "Meu deus do céu… Não consigo pensar em mais nada pra fazer…",
            "My god… I can’t think of anything else to do…",
            {{"Next", "air_cafe_right4"}, {"Close", nullptr}}};
    }
    SCRIPT("air_cafe_right4", any)
    {
        scr.activate_dialog = true;
        scr.dialog = Dialog{"-Chatter-",
                            "Da próxima vez, quero pegar um voo direto!",
                            "Next time, I want to get a direct flight!",
                            {{"Close", nullptr}}};
    }
#pragma endregion
#pragma region air_cafe_top
    SCRIPT("air_cafe_top", interacted)
    {
        scr.activate_dialog = true;
        scr.dialog = Dialog{
            "-Chatter-",
            "Como eu queria poder ficar mais tempo por aqui… Amo viajar!",
            "I wish I could stay longer… I love travelling!",
            {{"Close", nullptr}}};
    }
#pragma endregion
#pragma region air_cafe_left
    SCRIPT("air_cafe_left", interacted)
    {
        scr.activate_dialog = true;
        scr.dialog =
            Dialog{"-Chatter-",
                   "Esse cappuccino de canela é uma delícia! O croissant "
                   "podia ser mais fresco…",
                   "This cinnamon cappuccino is delicious! The croissant "
                   "could be fresher, though…",
                   {{"Close", nullptr}}};
    }
#pragma endregion
#pragma region taxist_a
    SCRIPT("taxist_a", interacted)
    {
        scr.activate_dialog = true;
        scr.dialog = Dialog{"Taxista",
                            "Boa tarde!",
                            "Good afternoon!",
                            {{"Next", "taxist_a2"}, {"Close", nullptr}}};
    }
    SCRIPT("taxist_a2", any)
    {
        scr.activate_dialog = true;
        scr.dialog = Dialog{"Lexi",
                            "Boa tarde!",
                            "Good afternoon!",
                            {{"Next", "taxist_a3"}, {"Close", nullptr}}};
    }
    SCRIPT("taxist_a3", any)
    {
        scr.activate_dialog = true;
        scr.dialog = Dialog{"Lexi",
                            "(Should I take a cab?)",
                            "",
                            {{"Yes", "taxist_a4"}, {"No", "later"}}};
    }
    SCRIPT("taxist_a4", any)
    {
        scr.activate_dialog = true;
        scr.dialog = Dialog{"Lexi",
                            "Vamos pra Rua Tijuco Preto, 63!",
                            "Let’s go to 63 Tijuco Preto Street!",
                            {{"Next", "taxist_a5"}}};
    }
    SCRIPT("taxist_a5", any)
    {
        scr.activate_dialog = true;
        scr.dialog =
            Dialog{"Taxista", "Vamos la!", "Let’s go!", {{"Go", "taxist_a6"}}};
    }
    SCRIPT("taxist_a6", any)
    {
        scr.target_pos = {-12, 20};
        scr.activate_dialog = true;
        scr.dialog =
            Dialog{"Taxista",
                   "Número sessenta e três! Prontinho! Ficou… R$ 120,00! "
                   "Vai pagar no cartão?",
                   "Number sixty-three! There we are! It’s… R$ 120,00! Are "
                   "you paying with a card?",
                   {{"Next", "taxist_a7"}}};
    }
    SCRIPT("taxist_a7", any)
    {
        scr.activate_dialog = true;
        scr.dialog =
            Dialog{"Lexi", "Crédito!", "Credit!", {{"Next", "taxist_a8"}}};
    }
    SCRIPT("taxist_a8", any)
    {
        scr.currency -= 30000;
        scr.activate_dialog = true;
        scr.dialog = Dialog{
            "Taxista",
            "[30 000 ₷ debited from balance]\nMuito obrigado! Boa tarde!",
            "Thank you very much! Good afternoon!",
            {{"Next", "taxist_a9"}}};
    }
    SCRIPT("taxist_a9", any)
    {
        scr.activate_dialog = true;
        scr.dialog = Dialog{"Lexi",
                            "Obrigada! Pra você também!",
                            "Thank you! You too!",
                            {{"Close", nullptr}}};
    }
#pragma endregion
#pragma region walter
    SCRIPT("walter", interacted)
    {
        scr.activate_dialog = true;
        scr.dialog = Dialog{"Lexi", "Walter?", "", {{"Next", "walter2"}}};
    }
    SCRIPT("walter2", any)
    {
        scr.activate_dialog = true;
        scr.dialog = Dialog{"Walter",
                            "Lexi? Tudo bem com você? Boa tarde!",
                            "Lexi? How are you doing? Good afternoon!",
                            {{"Next", "walter3"}}};
    }
    SCRIPT("walter3", any)
    {
        scr.activate_dialog = true;
        scr.dialog = Dialog{"Lexi",
                            "Boa tarde! Bem, e você?",
                            "Good afternoon! I’m fine, and you?",
                            {{"Yes", "walter4"}}};
    }
    SCRIPT("walter4", any)
    {
        scr.activate_dialog = true;
        scr.dialog = Dialog{
            "Walter",
            "Tudo bem, graças a deus. Foi bem de viagem? Deu tudo certo?",
            "All right, thank god. Did you travel well? Did everything go as "
            "planned?",
            {{"Next", "walter5"}}};
    }
    SCRIPT("walter5", any)
    {
        scr.activate_dialog = true;
        scr.dialog = Dialog{"Lexi",
                            "Sim, sim, estou ótima. Consegui tirar o dinheiro.",
                            "Yeah, I’m great. I managed to withdraw the money.",
                            {{"Next", "walter6"}}};
    }
    SCRIPT("walter6", any)
    {
        scr.activate_dialog = true;
        scr.dialog =
            Dialog{"Lexi", "Aqui está!", "Here you go!", {{"Next", "walter7"}}};
    }
    SCRIPT("walter7", any)
    {
        scr.data[1] = 1;
        scr.reals -= 1900;
        scr.activate_dialog = true;
        scr.dialog =
            Dialog{"Walter",
                   "Muito obrigado, viu? Mais uma vez, desculpa da "
                   "maquininha! Aqui está a sua chave!",
                   "Thank you very much! Once again, I’m sorry for the card "
                   "reader! Here’s your key!",
                   {{"Close", nullptr}}};
    }
#pragma endregion
#pragma region gohome
    SCRIPT("gohome", interacted)
    {
        if (scr.data[1])
        {
            scr.activate_dialog = true;
            scr.dialog = Dialog{
                "Lexi",
                "(After such a long flight, I’m feeling quite jetlagged… Even "
                "though it’s the middle of the day,\n I should probably crash "
                "into bed.)",
                "",
                {{"Next", "gohome2"}}};
        }
        else
        {
            scr.activate_dialog = true;
            scr.dialog = Dialog{
                "Lexi", "(I don't have the key)", "", {{"Close", nullptr}}};
        }
    }
    SCRIPT("gohome2", any)
    {
        scr.activate_dialog = true;
        scr.dialog = Dialog{
            "-------",
            "-The end-",
            "",
        };
    }

#pragma endregion
#pragma region exchange
    SCRIPT("exchange", interacted)
    {
        scr.activate_dialog = true;
        scr.dialog = Dialog{"Caixa",
                            "Boa tarde!",
                            "Good afternoon!",
                            {{"Next", "exchange2"}, {"Close", nullptr}}};
    }
    SCRIPT("exchange2", any)
    {
        scr.activate_dialog = true;
        scr.dialog = Dialog{"Lexi",
                            "Boa tarde!",
                            "Good afternoon!",
                            {{"Next", "exchange3"}, {"Close", nullptr}}};
    }
    SCRIPT("exchange3", any)
    {
        scr.activate_dialog = true;
        scr.dialog =
            Dialog{"Lexi",
                   "(Oh my god, look at those exchange rates! Not in a "
                   "million years…)",
                   "",
                   {{"Close", nullptr}}};
    }
#pragma endregion
#pragma endregion
#pragma region coffeeguy
    SCRIPT("coffeeguy", interacted)
    {
        scr.activate_dialog = true;
        scr.dialog = Dialog{
            "-Chatter-",
            "Espero que meu café chegue logo. O motorista já está vindo…",
            "I hope my coffee is ready soon. The driver’s already coming…",
            {{"Close", nullptr}}};
    }
#pragma endregion
#pragma region air_guard_after
    SCRIPT("air_guard_after", walked_on)
    {
        if (scr.data[DATA_GUARD] != 0)
        {
            scr.backtrack = true;
            scr.activate_dialog = true;
            scr.dialog =
                Dialog{"Guard",
                       "Desculpa! Aqui é só desembarque de passageiros…",
                       "I’m sorry! This way is only for arriving passengers…",
                       {{"Next", "air_guard_area2"}, {"Close", nullptr}}};
        }
    }
#pragma endregion
#pragma region atmex
    SCRIPT("atmex", interacted)
    {
        if (scr.currency < 200000)
        {
            scr.dialog = Dialog{
                "Lexi", "(I withdrawn enough)", "", {{"Close", nullptr}}};
            scr.activate_dialog = true;
            return nullptr;
        }
        scr.activate_dialog = true;
        scr.dialog = Dialog{
            "Lexi",
            "(Sweet, a free ATM!)\n(The rent for the first month will be "
            "R$ 1.900,00… I guess I need to withdraw at least that much…)",
            "",
            {{"Next", "atmex2"}, {"Close", nullptr}}};
    }
    SCRIPT("atmex2", any)
    {
        scr.activate_dialog = true;
        scr.dialog = Dialog{
            "ATM",
            "RATE (TAX INCLUDED): R$ 1,00 = S$ 415\nAMOUNT TO CONVERT:",
            "",
            {{"R$100", (char *)(scr.currency < 41500 ? "broke" : "atmex100")},
             {"R$500", (char *)(scr.currency < 207500 ? "broke" : "atmex500")},
             {"Close", nullptr}}};
    }
    SCRIPT("atmex100", any)
    {
        scr.currency -= 41500;
        scr.reals += 100;
    }
    SCRIPT("atmex500", any)
    {
        scr.currency -= 207500;
        scr.reals += 500;
    }
#pragma endregion
#pragma region atmchp
    SCRIPT("atmchp", interacted)
    {
        if (scr.currency < 200000)
        {
            scr.dialog = Dialog{
                "Lexi", "(I withdrawn enough)", "", {{"Close", nullptr}}};
            scr.activate_dialog = true;
            return nullptr;
        }

        scr.activate_dialog = true;
        scr.dialog = Dialog{
            "Lexi",
            "(Sweet, a free ATM!)\n(The rent for the first month will be "
            "R$ 1.900,00… I guess I need to withdraw at least that much…)",
            "",
            {{"Next", "atmchp2"}, {"Close", nullptr}}};
    }
    SCRIPT("atmchp2", any)
    {
        scr.activate_dialog = true;
        scr.dialog = Dialog{
            "ATM",
            "RATE (TAX INCLUDED): R$ 1,00 = S$ 300\nAMOUNT TO CONVERT:",
            "",
            {{"R$100", (char *)(scr.currency < 30000 ? "broke" : "atmchp100")},
             {"R$500", (char *)(scr.currency < 150000 ? "broke" : "atmchp500")},
             {"Close", nullptr}}};
    }
    SCRIPT("atmchp100", any)
    {
        scr.currency -= 30000;
        scr.reals += 100;
    }
    SCRIPT("atmchp500", any)
    {
        scr.currency -= 150000;
        scr.reals += 500;
    }
#pragma endregion
#pragma region air_guard_area
    SCRIPT("air_guard_area", walked_on)
    {
        if (scr.data[DATA_GUARD] == 0)
        {
            scr.data[DATA_GUARD]++;
        }
        else
        {
            scr.backtrack = true;
            scr.activate_dialog = true;
            scr.dialog =
                Dialog{"Guard",
                       "Desculpa! Aqui é só desembarque de passageiros…",
                       "I’m sorry! This way is only for arriving passengers…",
                       {{"Next", "air_guard_area2"}, {"Close", nullptr}}};
        }
    }
    SCRIPT("air_guard_area2", any)
    {
        scr.activate_dialog = true;
        scr.dialog = Dialog{"Lexi",
                            "Ah! Desculpa…",
                            "Oh! Sorry…",
                            {{"Next", "air_guard_area3"}, {"Close", nullptr}}};
    }
    SCRIPT("air_guard_area3", any)
    {
        scr.activate_dialog = true;
        scr.dialog = Dialog{"Lexi",
                            "(I will pay more attention to what I’m doing…)",
                            "",
                            {{"Close", nullptr}}};
    }
#pragma endregion
#pragma region tehchnical
    SCRIPT("grasson", walked_on)
    {
        scr.activate_grass = true;
    }
    SCRIPT("grassoff", walked_on)
    {
        scr.activate_grass = false;
    }
    SCRIPT("broke", any)
    {
        scr.dialog = Dialog{"Lexi",
                            "(I dont have enough money to do that)",
                            "",
                            {{"Close", nullptr}}};
        scr.activate_dialog = true;
    }
    SCRIPT("broke", any)
    {
        scr.dialog = Dialog{"Lexi", "(Maybe later)", "", {{"Close", nullptr}}};
        scr.activate_dialog = true;
    }
    SCRIPT("1900quota", walked_on)
    {
        if (scr.reals < 1900)
        {
            scr.backtrack = true;
            scr.dialog = Dialog{
                "Lexi",
                "(Not yet, Lexi… I still need to figure out a way to get "
                "the rent money.)",
                "",
                {{"Close", nullptr}}};
            scr.activate_dialog = true;
        }
    }
#pragma endregion

    return nullptr;
}
