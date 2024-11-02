#include <iostream>
#include <unordered_map>
#include <string>
#include <random>
#include <chrono>
#include <fstream>

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
    const int max = 50, min = 0;

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

    void entrada(const string& nome, int qtd) {
        auto it = produtos.find(nome);
        if (it != produtos.end()) {
           // while (it->second.getQuantidade() + qtd > max){}
            it->second.adicionarEstoque(qtd);
        }
    }

    void saida(const string& nome, int qtd) {
        auto it = produtos.find(nome);
        if (it != produtos.end()) {
            //while (it->second.getQuantidade() - qtd < min){}
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

int main() {
    Estoque estoque;
    int i;
    const int vezes = 100, qtdEntrada = 1, qtdSaida = 1;

    lerProdutosArquivo("produtos.txt", estoque);
    
    auto inicio = chrono::high_resolution_clock::now();

    // Número de trabalhadores
    const int nt = 4;
    for(i = 0; i < nt; i++){
        simularEntrada(estoque, vezes, qtdEntrada);
    }

    for(i = 0; i < nt; i++){;
        simularSaida(estoque, vezes, qtdSaida);
    }

    auto fim = chrono::high_resolution_clock::now();
    chrono::duration<double> duracao = fim - inicio;

    estoque.exibirEstoque();
    cout << "\nTempo de execução: " << duracao.count() << " segundos" << endl;

    return 0;
}
