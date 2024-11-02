#include <iostream>
#include <unordered_map>
#include <string>
#include <random>
#include <chrono>

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
        //cout << nome << " adicionado ao estoque com quantidade: " << quantidadeInicial << " e preco: " << precoInicial << endl;
    }

    void entrada(const string& nome, int qtd) {
        auto it = produtos.find(nome);
        if (it != produtos.end()) {
           // while (it->second.getQuantidade() + qtd > max){}
            it->second.adicionarEstoque(qtd);
            //cout << "Adicionou " << qtd << " unidades de " << nome << " ao estoque." << endl;
        }
    }

    void saida(const string& nome, int qtd) {
        auto it = produtos.find(nome);
        if (it != produtos.end()) {
            //while (it->second.getQuantidade() - qtd < min){}
            it->second.retirarEstoque(qtd);
            //cout << "Removeu " << qtd << " unidades de " << nome << " do estoque." << endl;
        }
    }

    void novoPreco(const string& nome, float preco) {
        auto it = produtos.find(nome);
        if (it != produtos.end()) {
            it->second.atualizarPreco(preco);
            //cout << "Aturalizou preco do produto " << preco << " para " << preco << endl;
        }
    }

    void novoDesc(const string& nome, float desc) {
        auto it = produtos.find(nome);
        if (it != produtos.end()) {
            it->second.atualizarDesconto(desc);
            //cout << "Aturalizou preco do produto " << preco << " para " << preco << endl;
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
    for (int i = 0; i < vezes; i++) {
        estoque.entrada("ProdutoA", quantidade);
        estoque.entrada("ProdutoB", quantidade);
        estoque.entrada("ProdutoC", quantidade);
        estoque.entrada("ProdutoD", quantidade);
        estoque.entrada("ProdutoE", quantidade);
        estoque.novoPreco("ProdutoA", gerarValorAleatorio(6.0, 8.0));
        estoque.novoPreco("ProdutoB", gerarValorAleatorio(6.0, 8.0));
        estoque.novoPreco("ProdutoC", gerarValorAleatorio(7.0, 9.0));
        estoque.novoPreco("ProdutoD", gerarValorAleatorio(7.0, 9.0));
        estoque.novoPreco("ProdutoE", gerarValorAleatorio(8.0, 10.0));
        estoque.novoDesc("ProdutoA", gerarValorAleatorio(0.2, 0.5));
        estoque.novoDesc("ProdutoB", gerarValorAleatorio(0.2, 0.5));
        estoque.novoDesc("ProdutoC", gerarValorAleatorio(0.2, 0.5));
        estoque.novoDesc("ProdutoD", gerarValorAleatorio(0.2, 0.5));
        estoque.novoDesc("ProdutoE", gerarValorAleatorio(0.2, 0.5));
    }
}

void simularSaida(Estoque& estoque, int vezes, int quantidade) {
    for (int i = 0; i < vezes; i++) {
        estoque.saida("ProdutoA", quantidade);
        estoque.saida("ProdutoB", quantidade);
        estoque.saida("ProdutoC", quantidade);
        estoque.saida("ProdutoD", quantidade);
        estoque.saida("ProdutoE", quantidade);
        estoque.novoPreco("ProdutoA", gerarValorAleatorio(8.0, 10.0));
        estoque.novoPreco("ProdutoB", gerarValorAleatorio(8.0, 10.0));
        estoque.novoPreco("ProdutoC", gerarValorAleatorio(9.0, 11.0));
        estoque.novoPreco("ProdutoD", gerarValorAleatorio(9.0, 11.0));
        estoque.novoPreco("ProdutoE", gerarValorAleatorio(10.0, 12.0));
        estoque.novoDesc("ProdutoA", gerarValorAleatorio(0.0, 0.2));
        estoque.novoDesc("ProdutoB", gerarValorAleatorio(0.0, 0.2));
        estoque.novoDesc("ProdutoC", gerarValorAleatorio(0.0, 0.2));
        estoque.novoDesc("ProdutoD", gerarValorAleatorio(0.0, 0.2));
        estoque.novoDesc("ProdutoE", gerarValorAleatorio(0.0, 0.2));
    }
}

int main() {
    Estoque estoque;
    int i;
    const int vezes = 50, qtdEntrada = 1, qtdSaida = 1;

    estoque.adicionarProduto("ProdutoA", 0, 5.99, 0.0);
    estoque.adicionarProduto("ProdutoB", 0, 6.66, 0.0);
    estoque.adicionarProduto("ProdutoC", 0, 6.99, 0.0);
    estoque.adicionarProduto("ProdutoD", 0, 9.69, 0.0);
    estoque.adicionarProduto("ProdutoE", 0, 16.66, 0.0);
    
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
