extends Resource
class_name Klass

var id: StringName

@export var name: String

@export var available_followers: Array[BeingGenTemplate]

@export var combat_multipliers: CombatMultipliers = CombatMultipliers.new()

@export var available_spells: Array[StringName]

func validate():
	for follower in available_followers:
		assert(Global.races[follower.race_id] is UncontrollableRace)


# class shouldn't change how much you are capable of carrying
# class shouldn't give any type of damage reduction
# class should give unique unlockable abilities (like diablo abilities?)
# RACES shouddn't give unique unlockable abilities, instead, create a class only available for that race which has the abilities you need
#make it inherit from the common basic class with no abilities if you need the general type (like Warrior)
