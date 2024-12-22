extends Resource
class_name Klass

var mid: StringName

@export var mname: String

@export var mavailable_followers: Dictionary[BeingGenTemplate, float] = {}#use a null as key to specify weight of getting no follower

@export var mcombat_multipliers: CombatMultipliers = CombatMultipliers.new()

@export var mavailable_spells: Array[StringName] = []

func validate() -> bool: 
	var sum_of_weights: float = 0
	for follower: BeingGenTemplate in mavailable_followers.keys():
		var weight: float = mavailable_followers[follower]
		sum_of_weights += weight
		if weight < 0: return false
		if not follower.mrace is UncontrollableRace: return false
	
	if not mavailable_followers.keys().is_empty() and sum_of_weights <= 0:
		push_error("sum of weights for available followers in klass %s is zero"%[mid])
		return false
	return true


# class shouldn't change how much you are capable of carrying
# class shouldn't give any type of damage reduction
# class should give unique unlockable abilities (like diablo abilities?)
# RACES shouddn't give unique unlockable abilities, instead, create a class only available for that race which has the abilities you need
#make it inherit from the common basic class with no abilities if you need the general type (like Warrior)
