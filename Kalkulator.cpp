#include <iostream>
#include <set>
#include <stack>
#include <string>
#include <vector>

/*
 * Ustalmy podstawowe typy komend jakie będzie obsługiwał kalkulator: operatory
 * i wartości (na których operatory będą operować).
 */
enum class Command_kind {
    Operator,
    Value,
};

/*
 * Następnie przygotujmy strukturę, która będzie reprezentowała "polecenie"
 * wydane kalkulatorowi. Musimy przechować w tej strukturze informację o tym
 * czy polecenie jest operatorem czy wartością, orz informację o tym jaka jest
 * zawartość polecenia (żeby móc sprawdzić *jakim* operatorem jest lub *jaką*
 * wartość ma).
 */
struct Command {
    Command_kind const kind;
    std::string const value;

    /*
     * Konstruktor typu Command. Jest to specjalna funkcja zajmująca się
     * "przygotowywaniem" wartości danego typu. Przyjmuje parametry, musi się
     * nazywać tak samo jak typ, i nie ma bezpośredniej wartości zwracanej.
     */
    Command(Command_kind const k, std::string v)
        : kind{k}
        , value{std::move(v)}
    {}
};

/*
 * Ta funkcja będzie pobierać od użytkownika napisy i przerabiać je na wektory
 * poleceń. Dzięki temu reszta programu będzie mogła operować na poleceniach, a
 * nie napisach.
 */
std::vector<Command> get_input()
{
    std::string line;
    std::getline(std::cin, line);

    /*
     * Upewnijmy się, że nasze dane wejściowe zawsze są zakończone spacją.
     * Uprości to pętlę dzielącą napisy na elementy.
     */
    line += ' ';

    /*
     * Jeśli standardowy strumień wejścia został zamknięty to zasygnalizujmy to
     * kalkulatorowi automatycznie dodając polecenie wyjścia.
     */
    if (std::cin.eof()) {
        line += "q ";
    }

    std::vector<std::string> elements;
    {
        /*
         * Ta pętla podzieli napis na elementy, używając spacji jako znaku
         * podziału. Elementy dodajemy tylko jeśli nie są puste!
         */
        std::string token;
        for (auto const each : line) {
            if (each == ' ' and not token.empty()) {
                elements.push_back(token);
                token = "";
                continue;
            }

            /*
             * Spacje nie są interesujące. Pomińmy je przy budowaniu
             * elementów.
             */
            if (each == ' ') {
                continue;
            }

            token += each;
        }
    }

    /*
     * Zbiór operatorów rozpoznawanych przez kalkulator. Jakikolwiek inny
     * element będzie uznany za wartość. Nowe operatory należy dodawać tutaj.
     */
    std::set<std::string> const operators {
        "q",
        "p",
        "+",
        "-",
        "*",
        "/",
    };

    /*
     * Ta pętla przetworzy wektor napisów na wektor poleceń. Reszta programu nie
     * będzie musiała analizować zawartości napisu za każdym razem dzięki temu,
     * że w tym miejscu przetworzymy nasze dane wejściowe.
     */
    std::vector<Command> commands;
    for (auto const& each : elements) {
        /*
         * Element jest albo operatorem albo wartością. Inne przypadki są
         * nieobsługiwane i nie implementujemy ich tutaj.
         */
        if (operators.count(each)) {
            commands.push_back(Command(Command_kind::Operator, each));
        } else {
            commands.push_back(Command(Command_kind::Value, each));
        }
    }

    return commands;
}

void execute_operator(std::stack<int>& calculator_stack, Command const& command)
{
    if (command.value == "p") {
        std::cout << calculator_stack.top() << std::endl;
    } else if (command.value == "+") {
        auto const a = calculator_stack.top();
        calculator_stack.pop();

        auto const b = calculator_stack.top();
        calculator_stack.pop();

        calculator_stack.push(a + b);
    } else if (command.value == "*") {
        auto const a = calculator_stack.top();
        calculator_stack.pop();

        auto const b = calculator_stack.top();
        calculator_stack.pop();

        calculator_stack.push(a * b);
    }
}

int main()
{
    /*
     * Stos, na którym przechowywane są wartości, na których pracować będzie
     * nasz kalkulator.
     */
    std::stack<int> calculator_stack;

    while (true) {
        /*
         * W głównej pętli naszego kalkulatora będziemy za każdym razem pobierać
         * polecenia, a potem wykonywać ich porcję. Pętla powinna wykonywać się
         * tak długo jak kalkulator nie napotka polecenia wyjścia.
         */
        auto const commands = get_input();
        for (auto const each : commands) {
            switch (each.kind) {
                case Command_kind::Operator:
                    if (each.value == "q") {
                        /*
                         * Polecenie "q" oznacza żądanie wyjścia (quit) z
                         * kalkulatora. Zakończmy więc pracę.
                         */
                        return 0;
                    }

                    /*
                     * W innym przypadku - wywołajmy funkcję implementującą
                     * operatory.
                     */
                    execute_operator(calculator_stack, each);
                    break;
                case Command_kind::Value:
                    /*
                     * Wartości są po prostu wpisywane na stos i czekają aż
                     * jakiś operator je wykorzysta.
                     */
                    calculator_stack.push(std::stoi(each.value));
                    break;
            }
        }
    }

    return 0;
}
