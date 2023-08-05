extends Resource
class_name Class

@export var id: String

@export var name: String

@export var available_followers: Array[UncontrollableRace]



@export var available_spells: Array[String]

# class should have a hp multiplier
# class should have a mana multiplier
# class should change outgoing melee damage multiplier or magic damage multiplayer

# class shouldn't change how much you are capable of carrying
# class shouldn't give any type of damage reduction
# class should give unique unlockable abilities (like diablo abilities?)
# races shouddn't give unique unlockable abilities, instead, create a class only available for that race which has the abilities you need
#make it inherit from the common basic class with no abilities if you need the general type (like Warrior)
