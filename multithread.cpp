#include <iostream>
#include <unordered_map>
#include <string>
#include <random>
#include <chrono>
#include <vector>
#include <thread>
#include <fstream>
#include <cstdlib>

using namespace std;

class Produto {
private:
    string nome;
    int quantidade;
    float valor;
    float desconto;

public:
    Produto(string nome, int quantidade, float valor, float desconto) : nome(nome), quantidade(quantidade), valor(valor), desconto(desconto) {}
    ~Produto() {}

    void adicionarEstoque(int qtd) {
        quantidade += qtd;
    }

    void retirarEstoque(int qtd) {
        quantidade -= qtd;
    }

    void atualizarPreco(float preco) {
        valor = preco;
    }

    void atualizarDesconto(float desc) {
        desconto = desc;
    }

    int getQuantidade() const { return quantidade; }
    string getNome() const { return nome; }
    float getPreco() const { 
        return valor; 
    }
    float getDesconto() const { return desconto; }
};


class Estoque {
private:
    unordered_map<string, Produto> produtos;
    const int max = 50;
    const int min = 0;

public:
    Estoque() {}

    ~Estoque() {}

    void adicionarProduto(const string& nome, int quantidadeInicial, float precoInicial, float descInicial) {
        produtos.emplace(nome, Produto(nome, quantidadeInicial, precoInicial, descInicial));
    }

    void removerProduto(const string& nome) {
        auto it = produtos.find(nome);
        if (it != produtos.end()) {
            produtos.erase(it);
        }
    }

    unordered_map<string, Produto>& getProdutos() {
        return produtos;
    }

    // As funções de entrada e saída criam o problema do produtor / consumidor
    void entrada(const string& nome, int qtd) {;
        auto it = produtos.find(nome);
        if (it != produtos.end()) { // Descomente o while para colocar uma verificação de limite
          //  while (it->second.getQuantidade() + qtd > max){}
            it->second.adicionarEstoque(qtd);
        }
    }

    void saida(const string& nome, int qtd) {
        auto it = produtos.find(nome);
        if (it != produtos.end()) {
          //  while (it->second.getQuantidade() - qtd < min){}
            it->second.retirarEstoque(qtd);
        }
    }

    void novoPreco(const string& nome, float preco) {
        auto it = produtos.find(nome);
        if (it != produtos.end()) {
            it->second.atualizarPreco(preco);
        }
    }

    void novoDesc(const string& nome, float desc) {
        auto it = produtos.find(nome);
        if (it != produtos.end()) {
            it->second.atualizarDesconto(desc);
        }
    }

    void exibirEstoque() const {
        cout << "\n--- Estoque Atual ---\n";
        for (const auto& pair : produtos) {
            cout << "Produto: " << pair.second.getNome() << " | Quantidade: " << pair.second.getQuantidade();
            cout << " | Preco: " << pair.second.getPreco() << " | Desconto: " << pair.second.getDesconto() << endl;
        }
    }
};

float gerarValorAleatorio(float min, float max) {
    static random_device rd;                           
    static mt19937 gen(rd());                          
    uniform_real_distribution<> dist(min, max);   
    return dist(gen);
}

void simularEntrada(Estoque& estoque, int vezes, int quantidade) {
    auto& produtos = estoque.getProdutos();
    for (int i = 0; i < vezes; i++) {
        // Percorre unordered_map produtos modificando a quantidade, preço e desconto
        for (auto& [nome, produto] : produtos) {
            estoque.entrada(nome, quantidade);
            estoque.novoPreco(nome, gerarValorAleatorio(5.0, 11.0));
            estoque.novoDesc(nome, gerarValorAleatorio(0.1, 0.5));
        }
    }
}

void simularSaida(Estoque& estoque, int vezes, int quantidade) {
    auto& produtos = estoque.getProdutos();
    for (int i = 0; i < vezes; i++) {
        // Percorre unordered_map produtos modificando a quantidade, preço e desconto
        for (auto& [nome, produto] : produtos) {
            estoque.saida(nome, quantidade);
            estoque.novoPreco(nome, gerarValorAleatorio(10.0, 20.0));
            estoque.novoDesc(nome, gerarValorAleatorio(0.0, 0.2));
        }
    }
}

void lerProdutosArquivo(const string& nomeArquivo, Estoque& estoque) {
    ifstream arquivo(nomeArquivo);
    if (!arquivo) {
        cerr << "Erro ao abrir o arquivo " << nomeArquivo << endl;
        return;
    }

    string nome;
    int quantidade;
    float preco, desconto;
    while (arquivo >> nome >> quantidade >> preco >> desconto) {
        estoque.adicionarProduto(nome, quantidade, preco, desconto);
    }
    arquivo.close();
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Uso: " << argv[0] << " <vezes> <nt>" << std::endl;
        return 1;
    }

    int vezes = atoi(argv[1]);
    // número de threads = número de trabalhadores
    int nt = atoi(argv[2]);

    Estoque estoque;
    int qtdEntrada = 1, qtdSaida = 1, i;

    lerProdutosArquivo("produtos.txt", estoque);

    auto inicio = chrono::high_resolution_clock::now();

    vector<thread> threads;
    for(i = 0; i < nt; i++){
        threads.emplace_back(simularEntrada, ref(estoque), vezes, qtdEntrada);
        threads.emplace_back(simularSaida, ref(estoque), vezes, qtdSaida);
    }
    
    for (auto& t : threads) {
        t.join();
    }

    auto fim = chrono::high_resolution_clock::now();
    chrono::duration<double> duracao = fim - inicio;

    estoque.exibirEstoque();
    cout << "\nTempo de execução: " << duracao.count() << " segundos" << endl;

    return 0;
}