# CER 简单物品随机(Simple CER Item Randomizer)

## 介绍

- 本 mod 将 cer 中所有的法术、武器以及盔甲进行了随机，其余东西不会发生变化。MOD采用运行时交换 id 的形式，支持CER 2.13。
旨在解决在CER中不知道走哪个流派的问题xD，强迫症福音。


## 使用教程

- 将 SCIR.dll,SCIR_config.json,SCIR_items.json 放到任意目录（注意要位于同一目录）
- 编辑 CER 的 config_eldenring.toml,在 external_dlls 内补上 SCIR.dll 的完整路径
- 编辑 SCIR_config.json，随意输入种子,0~2^31-1 内的数字均可
- 打开游戏创建新存档即可
- 打开游戏后 dll 所在目录会生成 ramdom_list_{seed}.txt 文件，用于告知ID替换规则。由于本mod不随机关键物品，因此不会出现卡关。唯一的用处在于
  如果你实在想获取某个武器/法术可以尝试查看该文件。

## 配置

```txt
{
    "debug_mode": false, //是否开启调试模式（会生成黑窗）
    "seed": 1000 //种子
}
```

## 局限性以及 bug

- 由于本 mod 只做了 id 交换，因此对于能重复获取的物品那么其被交换后的物品也将能重复获取（但是 CER 的武器盔甲以及法术基本都是一次固定获取，因此此处无关紧要）
- 虽然是运行时随机，但是从商店买的物品不会被随机
- **BUG** ,把武器（未升级）/盔甲丢地上再捡起来可以重新随机，没事请勿尝试。

## Credits
- https://www.nexusmods.com/eldenring/mods/3419
- https://github.com/Nordgaren/Erd-Tools
