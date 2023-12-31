using namespace types;
using namespace utils;

bool _roll_and_test(const int &now, int &primer, int per_ten_thounsad) {
  primer = (primer + now) % 65537;
  return (primer % 10000) < per_ten_thounsad;
}

void pet::openchest(name player) {

  require_auth(player);

  auto itr_account = accounts2.find(player);
  eosio_assert(itr_account != accounts2.end(), "account is not signed up");

  // add daily chest
  st_account2 account = *itr_account;
  bool is_daily_chest = (now() - account.actions[OPEN_DAILY_CHEST]) >= (24 * HOUR);
  if (is_daily_chest) {
    SEND_INLINE_ACTION( *this, issueitem, {_self,N(active)}, {player, asset{1, CHEST}, "dailychest"} );

    // updates last received dailychest
    accounts2.modify(itr_account, 0, [&](auto &r) {
      r.actions[OPEN_DAILY_CHEST] = now();
    });
  } else {
    eosio_assert(account.assets[CHEST] >= 1, "player has no chest to open");
  }

  // schedule reward in next 3 secs
  transaction trx{};
  trx.actions.emplace_back(
      permission_level{_self, N(active)},
      _self, N(chestreward),
      std::make_tuple(player, 1, "openchest")
  );
  trx.delay_sec = 1 + _random(3);
  trx.send(1, _self);
}

void pet::issueitem( name player, asset item, string reason) {
  require_auth(_self);

  auto itr_account = accounts2.find(player);
  eosio_assert(itr_account != accounts2.end(), "account is not signed up");

  eosio_assert(item.is_valid(), "Invalid item issue");
  eosio_assert(item.amount > 0, "Quantity must be positive");

  accounts2.modify(itr_account, 0, [&](auto &r) {
    r.add_asset(item.symbol, item.amount);
  });
}

void pet::issueitems( name player, vector<asset> items, string /* reason */ ) {
  require_auth(_self);

  auto itr_account = accounts2.find(player);
  eosio_assert(itr_account != accounts2.end(), "account is not signed up");

  accounts2.modify(itr_account, 0, [&](auto &r) {
    for (auto item : items) {
      eosio_assert(item.is_valid(), "Invalid item issue");
      eosio_assert(item.amount > 0, "Quantity must be positive");
      r.add_asset(item.symbol, item.amount);
    }
  });
}

void pet::chestreward(name player, uint8_t modifier, string reason) {
  require_auth(_self);
  
  auto itr_account = accounts2.find(player);
  eosio_assert(itr_account != accounts2.end(), "account is not signed up");

  // check chest balance
  st_account2 account = *itr_account;
  auto player_chests = account.assets[CHEST];
  eosio_assert(player_chests >= 1, "player has no chest to open");

  // reduce chest balance
  accounts2.modify(itr_account, 0, [&](auto &r) {
    r.assets[CHEST] = player_chests - 1;
  });

  int base = _random(65537);
  int primer = base;
  int timestamp = now();

  // calc rewards
  int candies = (1 + (primer % 4)) * modifier;
  bool energy_drink = _roll_and_test(timestamp, primer, 500 * modifier);
  bool small_hp_potion = _roll_and_test(timestamp, primer, 2000 * modifier);
  bool medium_hp_potion = _roll_and_test(timestamp, primer, 1000 * modifier);
  bool large_hp_potion = _roll_and_test(timestamp, primer, 500 * modifier);
  bool total_hp_potion = _roll_and_test(timestamp, primer, 100 * modifier);
  bool attack_elixir = _roll_and_test(timestamp, primer, 100 * modifier);
  bool super_attack_elixir = _roll_and_test(timestamp, primer, 50 * modifier);
  bool defense_elixir = _roll_and_test(timestamp, primer, 100 * modifier);
  bool super_defense_elixir = _roll_and_test(timestamp, primer, 50 * modifier);
  bool ihp_elixir = _roll_and_test(timestamp, primer, 100 * modifier);
  bool super_ihp_elixir = _roll_and_test(timestamp, primer, 50 * modifier);
  bool xp_bronze_scroll = _roll_and_test(timestamp, primer, 50 * modifier);
  bool xp_silver_scroll = _roll_and_test(timestamp, primer, 25 * modifier);
  bool xp_gold_scroll = _roll_and_test(timestamp, primer, 10 * modifier);
  bool sxp_bronze_scroll = _roll_and_test(timestamp, primer, 25 * modifier);
  bool sxp_silver_scroll = _roll_and_test(timestamp, primer, 10 * modifier);
  bool sxp_gold_scroll = _roll_and_test(timestamp, primer, 5 * modifier);
  bool revive_tome = _roll_and_test(timestamp, primer, 1 * modifier);

  // increment items balance
  vector<asset> items{};
  items.emplace_back(asset{candies, CANDY});
  if (energy_drink) items.emplace_back(asset{1, ENERGY_DRINK});
  if (small_hp_potion) items.emplace_back(asset{1, SMALL_HP_POTION});
  if (medium_hp_potion) items.emplace_back(asset{1, MEDIUM_HP_POTION});
  if (large_hp_potion) items.emplace_back(asset{1, LARGE_HP_POTION});
  if (total_hp_potion) items.emplace_back(asset{1, TOTAL_HP_POTION});
  if (attack_elixir) items.emplace_back(asset{1, INCREASED_ATTACK_ELIXIR});
  if (super_attack_elixir) items.emplace_back(asset{1, SUPER_ATTACK_ELIXIR});
  if (defense_elixir) items.emplace_back(asset{1, INCREASED_DEFENSE_ELIXIR});
  if (super_defense_elixir) items.emplace_back(asset{1, SUPER_DEFENSE_ELIXIR});
  if (ihp_elixir) items.emplace_back(asset{1, INCREASED_HP_ELIXIR});
  if (super_ihp_elixir) items.emplace_back(asset{1, SUPER_HP_ELIXIR});
  if (xp_bronze_scroll) items.emplace_back(asset{1, BRONZE_XP_SCROLL});
  if (xp_silver_scroll) items.emplace_back(asset{1, SILVER_XP_SCROLL});
  if (xp_gold_scroll) items.emplace_back(asset{1, GOLD_XP_SCROLL});
  if (sxp_bronze_scroll) items.emplace_back(asset{1, BRONZE_XP_SCROLL});
  if (sxp_silver_scroll) items.emplace_back(asset{1, SILVER_XP_SCROLL});
  if (sxp_gold_scroll) items.emplace_back(asset{1, GOLD_XP_SCROLL});
  if (revive_tome) items.emplace_back(asset{1, REVIVE_TOME});
  
  SEND_INLINE_ACTION( *this, issueitems, {_self,N(active)}, {player, items, reason} );
}

void pet::petconsume(uuid pet_id, symbol_type item) {

    auto itr_pet = pets.find(pet_id);
    eosio_assert(itr_pet != pets.end(), "E404|Invalid pet");
    st_pets pet = *itr_pet;

    require_auth(pet.owner);

    auto pc = _get_pet_config();

    eosio_assert(_is_alive(pet, pc) || item == REVIVE_TOME, "deads don't consume anything");
    eosio_assert(!pet.is_sleeping(), "pet is sleeping");

    // check the item balance
    auto itr_account = accounts2.find(pet.owner);
    eosio_assert(itr_account != accounts2.end(), "pet owner is not signed up");

    st_account2 account = *itr_account;
    auto balance = account.assets[item];
    eosio_assert(balance >= 1, "player does not have the item to consume");
    accounts2.modify(itr_account, 0, [&](auto &r) {
        r.assets[item] = balance - 1;
    });

    // execute consumption action here
    if (item == ENERGY_DRINK) {
      eosio_assert(pet.energy_drinks < MAX_DAILY_ENERGY_DRINKS, "you can only consume 10 energy drinks per day");
      pets.modify(itr_pet, 0, [&](auto &r) {
        r.energy_drinks = r.energy_drinks + 1;
        r.energy_used = 0;
      });
    } else {
      eosio_assert(false, "item not implemented");
    }

    // primer roller
    _random(10);
    
}
