
#include <iostream>
#include <unordered_map>
#include <string>
#include <random>
#include <chrono>
#include <vector>
#include <thread>

using namespace std;

class Produto {
private:
    string nome;
    int quantidade;
    float valor;
    float desconto;

public:
    Produto(string nome, int quantidade, float valor, float desconto) : nome(nome), quantidade(quantidade), valor(valor), desconto(desconto) {}
    ~Produto() {
        //cout << "Drestruido" << endl;
    }

    void adicionarEstoque(int qtd) {
        quantidade += qtd;
        //cout << "Adicioonando "<< qtd << endl;
    }

    void retirarEstoque(int qtd) {
        if (quantidade >= qtd) {
            quantidade -= qtd;
        } else {
            std::cout << "Estoque insuficiente para retirada de " << nome << "!" << std::endl;
        }
        //cout << "Removendo " << qtd << endl;
    }

    void atualizarPreco(float preco) {
        valor = preco;
        //cout << "Preco atualizado "<< preco << endl;
    }

    void atualizarDesconto(float desc) {
        desconto = desc;
    }

    int getQuantidade() const { return quantidade; }
    string getNome() const { return nome; }
    float getPreco() const { 
        return valor * (1.0 - desconto); 
    }
    float getDesconto() const { return desconto; }
};

class Estoque {
private:
    unordered_map<string, Produto> produtos;

public:
    Estoque() {
        //cout << "Estoque criado!" << endl;
    }

    ~Estoque() {
        //cout << "Estoque destruido!" << endl;
    }

    void adicionarProduto(const string& nome, int quantidadeInicial, float precoInicial, float descInicial) {
        produtos.emplace(nome, Produto(nome, quantidadeInicial, precoInicial, descInicial));
        //cout << "Produto " << nome << " adicionado ao estoque com quantidade " << quantidadeInicial << " e preco: " << precoInicial << endl;
    }

    void entrada(const string& nome, int qtd) {
        //unordered_map<string, Produto>::iterator it = produtos.find(nome);
        auto it = produtos.find(nome);
        if (it != produtos.end()) {
            it->second.adicionarEstoque(qtd);
           // cout << "Adicionou " << qtd << " unidades de " << nome << " ao estoque." << endl;
        } else {
            //cout << "Produto " << nome << " nao encontrado no estoque para entrada." << endl;
        }
    }

    void saida(const string& nome, int qtd) {
        //unordered_map<string, Produto>::iterator it = produtos.find(nome);
        auto it = produtos.find(nome);
        if (it != produtos.end()) {
            it->second.retirarEstoque(qtd);
            //cout << "Removeu " << qtd << " unidades de " << nome << " do estoque." << endl;
        } else {
            //cout << "Produto " << nome << " nao encontrado no estoque para saida." << endl;
        }
    }

    void novoPreco(const string& nome, float preco) {
        //unordered_map<string, Produto>::iterator it = produtos.find(nome);
        auto it = produtos.find(nome);
        if (it != produtos.end()) {
            it->second.atualizarPreco(preco);
           // cout << "Aturalizou preco do produto " << preco << " para " << preco << endl;
        } else {
            //cout << "Produto " << nome << " nao encontrado no estoque para saida." << endl;
        }
    }

    void novoDesc(const string& nome, float desc) {
        //unordered_map<string, Produto>::iterator it = produtos.find(nome);
        auto it = produtos.find(nome);
        if (it != produtos.end()) {
            it->second.atualizarDesconto(desc);
            //cout << "Aturalizou desconto do produto " << nome << " para " << desc << endl;
        } else {
            //cout << "Produto " << nome << " nao encontrado no estoque para saida." << endl;
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
        estoque.novoPreco("ProdutoA", gerarValorAleatorio(6.0, 12.0));
        estoque.novoPreco("ProdutoB", gerarValorAleatorio(6.0, 12.0));
        estoque.novoDesc("ProdutoA", gerarValorAleatorio(0.0, 1.0));
        estoque.novoDesc("ProdutoB", gerarValorAleatorio(0.0, 1.0));
    }
}

void simularSaida(Estoque& estoque, int vezes, int quantidade) {
    for (int i = 0; i < vezes; i++) {
        estoque.saida("ProdutoA", quantidade);
        estoque.saida("ProdutoB", quantidade);
        estoque.novoPreco("ProdutoA", gerarValorAleatorio(6.0, 12.0));
        estoque.novoPreco("ProdutoB", gerarValorAleatorio(6.0, 12.0));
        estoque.novoDesc("ProdutoA", gerarValorAleatorio(0.0, 1.0));
        estoque.novoDesc("ProdutoB", gerarValorAleatorio(0.0, 1.0));
    }
}

int main() {
    Estoque estoque;
    int i;

    estoque.adicionarProduto("ProdutoA", 0, 5.99, 0.0);
    estoque.adicionarProduto("ProdutoB", 0, 6.66, 0.0);

    auto inicio = chrono::high_resolution_clock::now();

    const int vezes = 50, qtdEntrada = 1, qtdSaida = 1;
    
    vector<thread> threads;
    threads.emplace_back(simularEntrada, ref(estoque), vezes, qtdEntrada);
    threads.emplace_back(simularSaida, ref(estoque), vezes, qtdSaida);
    
    for (auto& t : threads) {
        t.join();
    }

    auto fim = chrono::high_resolution_clock::now();
    chrono::duration<double> duracao = fim - inicio;

    estoque.exibirEstoque();
    cout << "\nTempo de execução: " << duracao.count() << " segundos" << endl;

    return 0;
}
