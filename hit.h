#ifndef HIT_H
#define HIT_H

class Hit
{
public:
    Hit() : hash_(0), x_(0), y_(0) {}

    //Constructors
    Hit(int hash, double x, double y) {
        hash_ = hash;
        x_ = x;
        y_ = y;
    }

    ~Hit() {}

    //Methods
    double getX() const {return x_;}
    double getY() const {return y_;}
    int getHash() const {return hash_;}

private:
    int hash_;
    double x_;
    double y_;
};

Q_DECLARE_METATYPE(Hit);

#endif // HIT_H
