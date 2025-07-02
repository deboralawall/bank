#include <assert.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>

#include "bank.hpp"

using json = nlohmann::json;
using namespace std;

enum class Action {
  Init,
  Deposit,
  Withdraw,
  Transfer,
  BuyInvestment,
  SellInvestment,
  Unknown
};

Action stringToAction(const std::string &actionStr) {
  static const std::unordered_map<std::string, Action> actionMap = {
      {"init", Action::Init},
      {"deposit_action", Action::Deposit},
      {"withdraw_action", Action::Withdraw},
      {"transfer_action", Action::Transfer},
      {"buy_investment_action", Action::BuyInvestment},
      {"sell_investment_action", Action::SellInvestment}};

  auto it = actionMap.find(actionStr);
  if (it != actionMap.end()) {
    return it->second;
  } else {
    return Action::Unknown;
  }
}

void printBankState(const BankState& state) {
  cout << "Balances: [";
  bool first_balance = true;
  for (const auto& [owner, amount] : state.balances) {
    if (!first_balance) {
      cout << ", ";
    }
    cout << "{" << owner << ": " << amount << "}";
    first_balance = false;
  }
  cout << "], ";

  cout << "Investments: [";
  bool first_investment = true;
  for (const auto& [id, investment] : state.investments) {
    if (!first_investment) {
      cout << ", ";
    }
    cout << "{ID: " << id << ", Owner: " << investment.owner
         << ", Amount: " << investment.amount << "}";
    first_investment = false;
  }
  cout << "], ";

  cout << "Next ID: " << state.next_id << endl;
}


int int_from_json(json j) {
  string s = j["#bigint"];
  return stoi(s);
}

map<string, int> balances_from_json(json j) {
  map<string, int> m;
  for (auto it : j["#map"]) {
    m[it[0]] = int_from_json(it[1]);
  }
  return m;
}

map<int, Investment> investments_from_json(json j) {
  map<int, Investment> m;
  for (auto it : j["#map"]) {
    m[int_from_json(it[0])] = {.owner = it[1]["owner"],
                               .amount = int_from_json(it[1]["amount"])};
  }
  return m;
}

BankState bank_state_from_json(json state) {
  map<string, int> balances = balances_from_json(state["balances"]);
  map<int, Investment> investments =
      investments_from_json(state["investments"]);
  int next_id = int_from_json(state["next_id"]);
  return {.balances = balances, .investments = investments, .next_id = next_id};
}


int main() {
  for (int i = 0; i < 5000; i++) {
    cout << "Trace #" << i << endl;
    std::ifstream f("traces/out" + to_string(i) + ".itf.json");
    json data = json::parse(f);

    // Estado inicial: começamos do mesmo estado incial do trace
    BankState bank_state =
        bank_state_from_json(data["states"][0]["bank_state"]);

    auto states = data["states"];
    for (auto state : states) {
      string action = state["mbt::actionTaken"];
      json nondet_picks = state["mbt::nondetPicks"];

      string error = "";

      // Próxima transição
    switch (stringToAction(action)) {
    case Action::Init: {
      cout << "initializing" << endl;
      break;
    }

    case Action::Deposit: {
      string depositor = nondet_picks["depositor"]["value"];
      int amount = int_from_json(nondet_picks["amount"]["value"]);
      error = deposit(bank_state, depositor, amount);
      break;
    }

    case Action::Withdraw: {
      string withdrawer = nondet_picks["withdrawer"]["value"];
      int amount = int_from_json(nondet_picks["amount"]["value"]);
      error = withdraw(bank_state, withdrawer, amount);
      break;
    }

    case Action::Transfer: {
      string sender = nondet_picks["sender"]["value"];
      string receiver = nondet_picks["receiver"]["value"];
      int amount = int_from_json(nondet_picks["amount"]["value"]);
      error = transfer(bank_state, sender, receiver, amount);
      break;
    }

    case Action::BuyInvestment: {
      string buyer = nondet_picks["buyer"]["value"];
      int amount = int_from_json(nondet_picks["amount"]["value"]);
      error = buy_investment(bank_state, buyer, amount);
      break;
    }

    case Action::SellInvestment: {
      string seller = nondet_picks["seller"]["value"];
      int id = int_from_json(nondet_picks["id"]["value"]);
      error = sell_investment(bank_state, seller, id);
      break;
    }

    default: {
      cout << "Ação desconhecida ou não mapeada: " << action << endl;
      error = "Unknown action";
      break;
    }
    }

      BankState expected_bank_state = bank_state_from_json(state["bank_state"]);

      //cout << "TODO: comparar o estado esperado com o estado obtido" << endl; ok
      cout << "-----------------EXPECTED: -------------------" << endl;
      printBankState(expected_bank_state);
      cout << "-----------------ACTUAL: ---------------------" << endl;
      printBankState(bank_state);
      cout << "----------------------------------------------" << endl;

      string expected_error = string(state["error"]["tag"]).compare("Some") == 0
                                  ? state["error"]["value"]
                                  : "";

      //cout << "TODO: comparar o erro esperado com o erro obtido" << endl; ok
      cout << "-----------------ERROR: ---------------------" << endl;
      if (error.empty()) {
        cout << "No error" << endl;
      } else {
        cout << "Error: " << error << endl; 
      }   
      cout << "-----------------EXPECTED ERROR: ---------------------" << endl;
      if (expected_error.empty()) {
        cout << "No error" << endl;
      } else {
        cout << "Expected Error: " << expected_error << endl;
      }
    }
    cout << "----------------------------------------------" << endl;
  }
  return 0;
}
