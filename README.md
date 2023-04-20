# Chargers

Chargers has a virtual class ICharger, responsible for taking some of the manual work of changing code between chargers -- in this case, GWP and ELCON. 
I also recently added a Status flags in order to better approach the get_faults() function in both chargers (not featured in ICharger due to differences between their faults).
GWP was updated to include macros from CAN. 
