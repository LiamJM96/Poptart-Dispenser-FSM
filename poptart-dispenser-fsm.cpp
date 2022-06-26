#include <iostream>
#include <vector>
#include <string>

using namespace std;

enum state { Out_Of_Poptart, No_Credit, Has_Credit, Dispenses_Poptart };	// enum variables which hold each state available for the dispenser
enum stateParameter { No_Of_Poptarts, Credit, Cost_Of_Poptart };	// enum variables which are used to hold data in a vector for each parameter e.g. amount of credit left

class StateContext;

class State
{
protected:
	StateContext* CurrentContext;	// pointer to the 'StateContext' class named 'CurrentContext'

public:
	// used to specify which state the dispenser is currently in
	State(StateContext* Context)	// constructor which accepts a 'StateContext' pointer as it's argument
	{
		CurrentContext = Context;	// set 'CurrentContext' to whatever 'Context' points to
	}

	virtual ~State(void) {};
	virtual void transition(void) {};	// used to trigger (transition) to different states
};

class StateContext
{
protected:
	State *CurrentState = nullptr;	// pointer to the 'State' class named 'CurrentState' initialised to nullptr
	int stateIndex = 0;				// specifies which state the dispenser is currently in e.g. 1 = No_Credit
	vector<State*> availableStates;	// creates a vector of pointers to states that are available in the CurrentContext
	vector<int> stateParameters;	// creates a vector of integers which holds data for each stateParameter using the enum as it's index

public:
	virtual ~StateContext(void);	// iterates through the available states and deletes the data stored within each state and parameter
	virtual void setState(state newState);	// sets the state currentstate to the state passed in the method call
	virtual int getStateIndex(void);		// returns the current 'stateIndex'
	virtual void setStateParam(stateParameter SP, int value);	// sets stateParameter 'SP' to the value within the 'value' variable in the vector e.g. storing credits
	virtual int getStateParam(stateParameter SP);	// returns the current amount stored within the 'SP' parameter (index) within the vector e.g. credit
};

StateContext::~StateContext(void)
{
	for (int i = 0; i < this->availableStates.size(); i++) delete this->availableStates[i];
	this->availableStates.clear();
	this->stateParameters.clear();
}

void StateContext::setState(state newState)
{
	this->CurrentState = availableStates[newState];
	this->stateIndex = newState;
	this->CurrentState->transition();
}

int StateContext::getStateIndex(void)
{
	return this->stateIndex;
}

void StateContext::setStateParam(stateParameter SP, int value)
{
	this->stateParameters[SP] = value;
}

int StateContext::getStateParam(stateParameter SP)
{
	return this->stateParameters[SP];
}

// used to prevent the user interacting with the dispenser when it's in the transition state
// all methods return an error message, this is useful to prevent two states being active at once
// which can cause unexpected errors e.g. in banking applications
class Transition
{
public:
	virtual bool insertMoney(int) { cout << "Error!" << endl; return false; }
	virtual bool makeSelection(int) { cout << "Error!" << endl; return false; }
	virtual bool moneyRejected(void) { cout << "Error!" << endl; return false; }
	virtual bool addPoptart(int) { cout << "Error!" << endl; return false; }
	virtual bool dispense(void) { cout << "Error!" << endl; return false; }
};

// used to control the current state of the poptart dispenser
class PoptartState : public State, public Transition
{
public:
	PoptartState(StateContext* Context) : State(Context) {}
};

// used to define what each method should do in the 'Out_Of_Poptarts' state
// as designed using the state diagram specified in the assignment brief
// changes state when poptarts are added to the dispenser
class OutOfPoptart : public PoptartState
{
public:
	OutOfPoptart(StateContext* Context) : PoptartState(Context) {}
	bool insertMoney(int money);
	bool makeSelection(int option);
	bool moneyRejected(void);
	bool addPoptart(int number);
	bool dispense(void);
};

// used to define what each method should do in the 'No_Credit' state
// as designed using the state diagram specified in the assignment brief
// changes state when credit is added to the dispenser
class NoCredit : public PoptartState
{
public:
	NoCredit(StateContext* Context) : PoptartState(Context) {}
	bool insertMoney(int money);
	bool makeSelection(int option);
	bool moneyRejected(void);
	bool addPoptart(int number);
	bool dispense(void);
};

// used to define what each method should do in the 'Has_Credit' state
// as designed using the state diagram specified in the assignment brief
// changes state when either the user selects a poptart for dispensing
// or if money is rejected due to insufficient credit
class HasCredit : public PoptartState
{
public:
	HasCredit(StateContext* Context) : PoptartState(Context) {}
	bool insertMoney(int money);
	bool makeSelection(int option);
	bool moneyRejected(void);
	bool addPoptart(int number);
	bool dispense(void);
};

// used to define what each method should do in the 'Dispenses_Poptart' state
// as designed using the state diagram specified in the assignment brief
// changes state when either the dispenser is out of poptarts
// or if the user is out of credit
// or if the user has sufficient credit and there are poptarts available in the dispenser to 'Has_Credits'
class DispensesPoptart : public PoptartState
{
public:
	DispensesPoptart(StateContext* Context) : PoptartState(Context) {}
	bool insertMoney(int money);
	bool makeSelection(int option);
	bool moneyRejected(void);
	bool addPoptart(int number);
	bool dispense(void);
};

// Superclass for both the 'Bases' and 'Fillings' storing data and the use of the methods for each subclass
// e.g. 'SpicyBase' can inherit from this class
class Product
{
	friend class Filling;	// allows the Filling class to access the private methods and variables of this class
protected:
	string product_description;	// description of the poptart base or filling e.g. Chocolate
	int itemCost = 0;	// cost of the poptart base or filling e.g. 50
	//virtual Product* ReturnNext(void);
	//virtual void RemoveHighestCostItem(Product* HighestItem);
public:
	virtual void consume(void);
	virtual int cost(void);				// returns the product cost of the specified Base selected
	virtual string description(void);	// returns the desciption of the specified Base selected
	//virtual Product* ReturnHighestCostItem(void);
	//virtual void RemoveHighestCostItem(void);
};

void Product::consume(void)
{
	cout << "Consuming..." << endl;
}

int Product::cost(void)
{
	return this->itemCost;
}

string Product::description(void)
{
	return this->product_description;
}

// Default Poptart class that each 'Base' can inherit from and override depending on the Base selected
class Poptart : public Product
{
public:
	Poptart(void)
	{
		this->product_description = "Poptart";
		this->itemCost = 50;
	}
};

// Plain Base that overrides the description and cost of the Poptart product
class PlainBase : public Poptart
{
public:
	PlainBase(void)
	{
		this->product_description = "Plain";
		this->itemCost = 100;
	}
};

// Spicy Base that overrides the description and cost of the Poptart product
class SpicyBase : public Poptart
{
public:
	SpicyBase(void)
	{
		this->product_description = "Spicy";
		this->itemCost = 150;
	}
};

// Chocolate Base that overrides the description and cost of the Poptart product
class ChocolateBase : public Poptart
{
public:
	ChocolateBase(void)
	{
		this->product_description = "Chocolate";
		this->itemCost = 200;
	}
};

// Coconut Base that overrides the description and cost of the Poptart product
class CoconutBase : public Poptart
{
public:
	CoconutBase(void)
	{
		this->product_description = "Coconut";
		this->itemCost = 200;
	}
};
// Fruity Base that overrides the description and cost of the Poptart product
class FruityBase : public Poptart
{
public:
	FruityBase(void)
	{
		this->product_description = "Fruity";
		this->itemCost = 200;
	}
};

// Filling subclass that uses the Decorator Pattern to add fillings to the Poptart bases
// and can be used multiple times to combine fillings
class Filling : public Poptart
{
protected:
	Product* filling;

public:
	Filling(Product* customerFilling);	// constructor that sets the new filling to the current poptart
	virtual int cost(void);				// returns the current cost plus the cost of the currently selected filling
	virtual string description(void);	// returns the current Poptart plus the new filling description e.g. Spicy(Base) + Banana(Filling) Poptart
	void addToPoptart(Product* customerFilling);	// adds the new filling to the current poptart
	~Filling(void);	// deconstructor that deletes the filling after it finishes executing

};

Filling::Filling(Product* customerPoptart)	// pointer to the 'Product' class named 'customerFilling'
{
	filling = customerPoptart;	// set 'filling' to whatever 'customerPoptart' points to
}

int Filling::cost(void)
{
	return this->itemCost + filling->cost();
}

string Filling::description(void)
{
	return filling->description() + " + " + this->product_description;
}

void Filling::addToPoptart(Product* customerPoptart)
{
	filling = customerPoptart;
}

Filling::~Filling(void)
{
	delete filling;
}

// Chocolate Filling decorator which can be created multiple times
// for different flavours without hardcoding or breaking existing code
// e.g. adding a new 'Curry' filling
class ChocolateFilling : public Filling
{
public:
	ChocolateFilling(Product* customerPoptart) : Filling(customerPoptart)
	{
		this->product_description = "Chocolate";
		this->itemCost = 20;
	}
};

class BananaFilling : public Filling
{
public:
	BananaFilling(Product* customerPoptart) : Filling(customerPoptart)
	{
		this->product_description = "Banana";
		this->itemCost = 50;
	}
};


class StrawberryFilling : public Filling
{
public:
	StrawberryFilling(Product* customerPoptart) : Filling(customerPoptart)
	{
		this->product_description = "Strawberry";
		this->itemCost = 50;
	}
};

class RaspberryFilling : public Filling
{
public:
	RaspberryFilling(Product* customerPoptart) : Filling(customerPoptart)
	{
		this->product_description = "Raspberry";
		this->itemCost = 50;
	}
};

class AppleFilling : public Filling
{
public:
	AppleFilling(Product* customerPoptart) : Filling(customerPoptart)
	{
		this->product_description = "Apple";
		this->itemCost = 50;
	}
};

class BlackberryFilling : public Filling
{
public:
	BlackberryFilling(Product* customerPoptart) : Filling(customerPoptart)
	{
		this->product_description = "Blackberry";
		this->itemCost = 50;
	}
};

class MapleFilling : public Filling
{
public:
	MapleFilling(Product* customerPoptart) : Filling(customerPoptart)
	{
		this->product_description = "Maple";
		this->itemCost = 100;
	}
};

class MarshmellowFilling : public Filling
{
public:
	MarshmellowFilling(Product* customerPoptart) : Filling(customerPoptart)
	{
		this->product_description = "Marshmellow";
		this->itemCost = 20;
	}
};

class CheeseFilling : public Filling
{
public:
	CheeseFilling(Product* customerPoptart) : Filling(customerPoptart)
	{
		this->product_description = "Cheese";
		this->itemCost = 70;
	}
};

class CheeseAndHamFilling : public Filling
{
public:
	CheeseAndHamFilling(Product* customerPoptart) : Filling(customerPoptart)
	{
		this->product_description = "Cheese and Ham";
		this->itemCost = 100;
	}
};

class CaramelFilling : public Filling
{
public:
	CaramelFilling(Product* customerPoptart) : Filling(customerPoptart)
	{
		this->product_description = "Caramel";
		this->itemCost = 20;
	}
};

class VanillaFilling : public Filling
{
public:
	VanillaFilling(Product* customerPoptart) : Filling(customerPoptart)
	{
		this->product_description = "Vanilla";
		this->itemCost = 50;
	}
};


class Poptart_Dispenser : public StateContext, public Transition
{
	friend class DispensesPoptart;	// allows the DispensesPoptart class to access the private methods and variables of this class
	friend class HasCredit;	// allows the HasCredit class to access the private methods and variables of this class
private:
	PoptartState* PoptartCurrentState = nullptr;	// pointer to PoptartState class initialised to nullptr (default values)
	bool itemDispensed = false;
	//indicates whether a product is there to be retrieved
	Product* DispensedItem = nullptr;
	bool itemRetrieved = false; //indicates whether a product has been retrieved
public:
	Poptart_Dispenser(int inventory_count);
	~Poptart_Dispenser(void);
	bool insertMoney(int money);
	bool makeSelection(int option);
	bool moneyRejected(void);
	bool addPoptart(int number);
	bool dispense(void);
	Product* getProduct(void);
	virtual void setStateParam(stateParameter SP, int value);
	virtual int getStateParam(stateParameter SP);
};

// constructor that is used when the object is initialised using a starting amount of poptarts available for its argument
Poptart_Dispenser::Poptart_Dispenser(int inventory_count)
{
	// poptart should start with an inventory count of 0
	// meaning it starts in the Out_Of_Poptart state
	// it should then add a poptart which should change the state
	// based on the current state it is in

	// add all available states to the 'availableStates' vector
	this->availableStates.push_back(new OutOfPoptart(this));
	this->availableStates.push_back(new NoCredit(this));
	this->availableStates.push_back(new HasCredit(this));
	this->availableStates.push_back(new DispensesPoptart(this));

	this->stateParameters.push_back(0);	// No of Poptarts
	this->stateParameters.push_back(0); // Credit

	this->setState(Out_Of_Poptart);	// setting starting state to 'Out_Of_Poptart'

	// if 'inventory_count' (poptarts) are greater than 0 when first initialising
	// add the poptarts using the 'addPoptart' method which also sets the
	// state to 'No_Credit'
	if (inventory_count > 0)
	{
		this->addPoptart(inventory_count);
	}
}

// deletes product poptart at end of execution
Poptart_Dispenser::~Poptart_Dispenser(void)
{
	if (!this->itemRetrieved)
	{
		delete this->DispensedItem;
	}
}

// sets the current state to the specified state
// then calls the insertMoney method which has a different function
// depending on the current state
bool Poptart_Dispenser::insertMoney(int money)
{
	PoptartCurrentState = (PoptartState*) this->CurrentState;
	return this->PoptartCurrentState->insertMoney(money);
}

// sets the current state to the specified state
// then calls the makeSelection method which has a different function
// depending on the current state
bool Poptart_Dispenser::makeSelection(int option)
{
	PoptartCurrentState = (PoptartState*) this->CurrentState;
	return this->PoptartCurrentState->makeSelection(option);
}

// sets the current state to the specified state
// then calls the moneyRejected method which has a different function
// depending on the current state
bool Poptart_Dispenser::moneyRejected(void)
{
	PoptartCurrentState = (PoptartState*) this->CurrentState;
	return this->PoptartCurrentState->moneyRejected();
}

// sets the current state to the specified state
// then calls the addPoptart method which has a different function
// depending on the current state
bool Poptart_Dispenser::addPoptart(int number)
{
	PoptartCurrentState = (PoptartState*) this->CurrentState;
	return this->PoptartCurrentState->addPoptart(number);
}

// sets the current state to the specified state
// then calls the dispense method which has a different function
// depending on the current state
bool Poptart_Dispenser::dispense(void)
{
	PoptartCurrentState = (PoptartState*) this->CurrentState;
	return this->PoptartCurrentState->dispense();
}

Product* Poptart_Dispenser::getProduct(void)
{
	if (this->itemDispensed)	// if item has been dispensed
	{
		this->itemDispensed = false;	// then set itemDispensed back to false
		this->itemRetrieved = true;		// and item retrieved to true as it has been dispensed
		return this->DispensedItem;		// return the DispensedItem (Poptart) object
	}

	return nullptr;	// else return nullptr
}


void Poptart_Dispenser::setStateParam(stateParameter SP, int value)
{
	if (SP == Cost_Of_Poptart) return;	// if StateParameter 'SP' is equal to the StateParameter 'Cost_Of_Poptart' exit method
	this->stateParameters[SP] = value;	// else stored the cost of the poptart in the vector using the current 'SP' index
}

int Poptart_Dispenser::getStateParam(stateParameter SP)
{
	if (SP == Cost_Of_Poptart)	// if StateParameter 'SP' is equal to the StateParameter 'Cost_Of_Poptart'
	{
		if (DispensedItem == nullptr) return 0;	// if 'DispensedItem' is nullptr, return 0
		return DispensedItem->cost();			// else return the current cost of the 'DispensedItem' poptart
	}
	return this->stateParameters[SP];	// else return current 'SP' index
}

// OutOfPoptart State
// Cannot insert money when the current state of 'OutOfPoptarts'
// insertMoney calls 'moneyRejected' in order to refund credit
bool OutOfPoptart::insertMoney(int money)
{
	cout << "Error! No poptarts left." << endl;
	this->moneyRejected();
	return false;
}

// Cannot select a poptart as there are no poptarts in the dispenser
bool OutOfPoptart::makeSelection(int option)
{
	cout << "Error! No poptarts to select from!" << endl;
	return false;
}

// No poptarts available therefore credit is rejected when entered
bool OutOfPoptart::moneyRejected(void)
{
	cout << "Refunding credit!" << endl;
	return true;
}

// Adds poptarts to the dispenser
// then changes state to 'No Credit'
bool OutOfPoptart::addPoptart(int number)
{
	this->CurrentContext->setStateParam(No_Of_Poptarts, number);	// inserts poptarts into the vector 'stateParam' using index 'No_Of_Poptarts'
	this->CurrentContext->setState(No_Credit);	// changes state to 'No_Credit'
	return true;
}

// Cannot dispense as there are no poptarts within the dispenser
bool OutOfPoptart::dispense(void)
{
	cout << "Error! No poptarts available to dispense." << endl;
	return false;

}

// NoCredit State
// Inserts credit to the dispenser
// then changes state to 'Has Credit'
bool NoCredit::insertMoney(int money)
{
	cout << "Inserting: " << money;
	this->CurrentContext->setStateParam(Credit, money);	// inserts credit into the vector 'stateParam' using index 'Credit'
	cout << "\nNew Total: " << money << endl;
	this->CurrentContext->setState(Has_Credit);	// changes state to 'Has_Credit'
	return true;
}


// Cannot make a selection as user
// has insufficient credit
bool NoCredit::makeSelection(int option)
{
	cout << "Error! Insufficient credit!" << endl;
	return false;
}


// NOT SURE ON COMMON SENSE
bool NoCredit::moneyRejected(void)
{
	cout << "Error! Cannot reject credit in this state!" << endl;
	return false;
}

// Cannot add poptarts from this state
// as dispenser already contains poptarts
bool NoCredit::addPoptart(int number)
{
	cout << "Error! Dispenser already contains poptarts!" << endl;
	return false;
}

// Cannot dispense poptarts as user
// has insufficient credit
bool NoCredit::dispense(void)
{
	cout << "Error! Insufficient credit!" << endl;
	return false;
}

// HasCredit State
// Inserts credit to the dispenser from the 'HasCredit' state
// which also adds to the current total amount
bool HasCredit::insertMoney(int money)
{
	cout << "Inserting: " << money;
	money = money + this->CurrentContext->getStateParam(Credit);	// 'money' is equal to 'money' + the current credit stored in the 'stateParam' vector using the index 'Credit'  
	this->CurrentContext->setStateParam(Credit, money);	// inserts the 'money' into the 'setStateParam' vector using index 'Credit'
	cout << "\n New Total: " << money << endl;
	this->CurrentContext->setState(Has_Credit);	// changes state to 'Has_Credit' as user now has sufficient credit
	return true;
}

// Allows the user to select their poptart base and filling
// using the 'option' argument ready for dispensing
// uses bitmasking in order to allow the user to select a base
// and multiple fillings using 1 option code argument
bool HasCredit::makeSelection(int option)
{
	cout << "Selection made!" << endl;
	if (!((Poptart_Dispenser*)this->CurrentContext)->itemRetrieved)	// if no poptart has been retrieved
	{
		delete ((Poptart_Dispenser*)this->CurrentContext)->DispensedItem;	// deletes the previous dispensed poptart
	}
	
	// selecting a base with multiple different fillings can be done via the use of bitmasking
	// e.g. if I wanted a poptart with the base plain (1) and the fillings blackberry (1024) and banana (64)
	// you pass '1089' to the option code allowing the selection of the base and multiple fillings specified

	// BASES
	// bases can only be selected once therefore the use of if else statements only are acceptable here
	if (option & 1)
	{
		((Poptart_Dispenser*)this->CurrentContext)->DispensedItem = new PlainBase();	// set the DispensedItem to the specified base
	}
	else if (option & 2)
	{
		((Poptart_Dispenser*)this->CurrentContext)->DispensedItem = new SpicyBase();
	}
	else if (option & 4)
	{
		((Poptart_Dispenser*)this->CurrentContext)->DispensedItem = new ChocolateBase();
	}
	else if (option & 8)
	{
		((Poptart_Dispenser*)this->CurrentContext)->DispensedItem = new CoconutBase();
	}
	else if (option & 16)
	{
		((Poptart_Dispenser*)this->CurrentContext)->DispensedItem = new FruityBase();
	}
	
	// FILLINGS
	// as fillings can be selected more than once, the use of if statements only are acceptable here
	if (option & 32)
	{
		// adds a filling to the DispensedItem (poptart/product) by creating a new object and casting it to DispensedItem
		((Poptart_Dispenser*)this->CurrentContext)->DispensedItem = new ChocolateFilling(((Poptart_Dispenser*)this->CurrentContext)->DispensedItem);
	}
	if (option & 64)
	{
		((Poptart_Dispenser*)this->CurrentContext)->DispensedItem = new BananaFilling(((Poptart_Dispenser*)this->CurrentContext)->DispensedItem);
	}
	if (option & 128)
	{
		((Poptart_Dispenser*)this->CurrentContext)->DispensedItem = new StrawberryFilling(((Poptart_Dispenser*)this->CurrentContext)->DispensedItem);
	}
	if (option & 256)
	{
		((Poptart_Dispenser*)this->CurrentContext)->DispensedItem = new RaspberryFilling(((Poptart_Dispenser*)this->CurrentContext)->DispensedItem);
	}
	if (option & 512)
	{
		((Poptart_Dispenser*)this->CurrentContext)->DispensedItem = new AppleFilling(((Poptart_Dispenser*)this->CurrentContext)->DispensedItem);
	}
	if (option & 1024)
	{
		((Poptart_Dispenser*)this->CurrentContext)->DispensedItem = new BlackberryFilling(((Poptart_Dispenser*)this->CurrentContext)->DispensedItem);
	}
	if (option & 2048)
	{
		((Poptart_Dispenser*)this->CurrentContext)->DispensedItem = new MapleFilling(((Poptart_Dispenser*)this->CurrentContext)->DispensedItem);
	}
	if (option & 4096)
	{
		((Poptart_Dispenser*)this->CurrentContext)->DispensedItem = new MarshmellowFilling(((Poptart_Dispenser*)this->CurrentContext)->DispensedItem);
	}
	if (option & 8192)
	{
		((Poptart_Dispenser*)this->CurrentContext)->DispensedItem = new CheeseFilling(((Poptart_Dispenser*)this->CurrentContext)->DispensedItem);
	}
	if (option & 16384)
	{
		((Poptart_Dispenser*)this->CurrentContext)->DispensedItem = new CheeseAndHamFilling(((Poptart_Dispenser*)this->CurrentContext)->DispensedItem);
	}
	if (option & 32768)
	{
		((Poptart_Dispenser*)this->CurrentContext)->DispensedItem = new CaramelFilling(((Poptart_Dispenser*)this->CurrentContext)->DispensedItem);
	}
	if (option & 65536)
	{
		((Poptart_Dispenser*)this->CurrentContext)->DispensedItem = new VanillaFilling(((Poptart_Dispenser*)this->CurrentContext)->DispensedItem);
	}
	
	((Poptart_Dispenser*)this->CurrentContext)->itemRetrieved = false;	// sets 'itemRetrieved' to false meaning that the poptart is ready to be retrieved from the dispenser
	this->CurrentContext->setState(Dispenses_Poptart);	// changes state to 'Dispenses_Poptart'
	return true;	// returns true meaning no errors
}

// if moneyRejected is called in the 'HasCredit' state
// then 
bool HasCredit::moneyRejected(void)
{
	cout << "Credit rejected!" << endl;
	this->CurrentContext->setStateParam(Credit, 0);	// sets the 'money' into the 'setStateParam' vector to 0 using index 'Credit'
	this->CurrentContext->setState(No_Credit);	// changes state to 'No_Credit'
	return true;	// returns true meaning no errors
}

// Cannot add poptarts as dispenser already contains poptarts
bool HasCredit::addPoptart(int number)
{
	cout << "Error! Dispenser already contains poptarts!" << endl;
	return false;	// returns false meaning an unexpected error has occurred
}


// Cannot dispense as user hasn't selected a poptart to be dispensed
bool HasCredit::dispense(void)
{
	cout << "Error! Please select poptart!" << endl;
	return false;	// returns false meaning an unexpected error has occurred
}

// DispensesPoptart State
// Cannot insertMoney as dispenser is preparing
// to dispense poptart
bool DispensesPoptart::insertMoney(int money)
{
	cout << "Error! Already dispensing poptart!" << endl;
	return false;	// returns false meaning an unexpected error has occurred
}


// Cannot makeSelection as user has already selected a poptart
// to be dispensed
bool DispensesPoptart::makeSelection(int option)
{
	cout << "Error! Already dispensing poptart!" << endl;
	return false;	// returns false meaning an unexpected error has occurred
}


// Cannot reject credit as dispenser is preparing
// to dispense poptart
bool DispensesPoptart::moneyRejected(void)
{
	cout << "Error! Already dispensing poptart!" << endl;
	return false;	// returns false meaning an unexpected error has occurred
}


// Cannot addPoptart as dispenser is already
// dispensing poptart
bool DispensesPoptart::addPoptart(int number)
{
	cout << "Error! Already dispensing poptart!" << endl;
	return false;	// returns false meaning an unexpected error has occurred
}

// 'Dispenses' poptart from the Poptart_Dispenser
// takes away the cost of the poptart from the users
// existing credit
bool DispensesPoptart::dispense(void)
{
	// checks to see if the user has enough credit to dispense the selected poptart
	if (this->CurrentContext->getStateParam(Credit) >= this->CurrentContext->getStateParam(Cost_Of_Poptart))
	{
		// outputs the description of the currently dispensed poptart
		cout << "Dispensing " << ((Poptart_Dispenser*)this->CurrentContext)->DispensedItem->description() << " Poptart.\n";

		// subtracts the cost of the poptart from the amount of credits available in the dispenser
		this->CurrentContext->setStateParam(Credit, this->CurrentContext->getStateParam(Credit)
			- this->CurrentContext->getStateParam(Cost_Of_Poptart));

		// removes the dispensed poptart from the dispenser by subtracting 1 from the available amount stored
		// in the 'stateParam' vector using index 'No_Of_Poptarts'
		this->CurrentContext->setStateParam(No_Of_Poptarts, this->CurrentContext->getStateParam(No_Of_Poptarts) - 1);

		// sets the bool value of itemDispensed to true indicating that the poptart has been dispensed
		((Poptart_Dispenser*)this->CurrentContext)->itemDispensed = true;
		
		// displays remaining credits
		cout << "Remaining credit: " << this->CurrentContext->getStateParam(Credit) << "." << endl;
	}
	else // else if there's not enough credit to dispense poptart
	{
		cout << "Error! Not enough money" << endl;
	}

	// if there's more than 1 credit left in the dispenser
	// set the current state to 'Has_Credit'
	if (this->CurrentContext->getStateParam(Credit) > 0)
	{
		this->CurrentContext->setState(Has_Credit);
	}
	else // else if the user has no credits
	{
		// set current state to 'No_Credit'
		this->CurrentContext->setState(No_Credit);
	}

	// if there are no poptarts left in the dispenser
	// set the current state to 'Out_Of_Poptart'
	if (this->CurrentContext->getStateParam(No_Of_Poptarts) == 0)
	{
		this->CurrentContext->setState(Out_Of_Poptart);
	}
	return true;
}

int main(void)
{
	Poptart_Dispenser* MyPoptart = new Poptart_Dispenser(0);

	MyPoptart->addPoptart(2);
	MyPoptart->insertMoney(5000);
	MyPoptart->makeSelection(55555);
	MyPoptart->dispense();
	return 0;
}