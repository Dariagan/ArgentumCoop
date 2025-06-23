extends Resource
class_name Klass

var mid: StringName

@export var mname: String

@export var mselectable_followers: Array[BeingGenTemplate]
@export var mmax_followers: int = 1
@export var mmax_bk_points: int = -1 

@export var mcombat_multipliers: CombatMultipliers = CombatMultipliers.new()

@export var mavailable_spells: Array[StringName] = []


# class shouldn't change how much you are capable of carrying
# class shouldn't give any type of damage reduction
# class should give unique unlockable abilities (like diablo abilities?)
# RACES shouddn't give unique unlockable abilities, instead, create a class only available for that race which has the abilities you need
#make it inherit from the common basic class with no abilities if you need the general type (like Warrior)
