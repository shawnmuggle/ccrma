class PlanetsController < ApplicationController

  def create
    @planet = Planet.create
    @planet.r = rand
    @planet.g = rand
    @planet.b = rand
    @planet.texture = "venusbump.jpg"
    
    @planet.trees.create :x => 0, :y => 1, :z => 0, :age => 3.0, :frequency => 220
    
    @planet.save
    
    render :text => "Successfully created a planet!\nPlanet ID: " + @planet.id.to_s
  end

  def post

  end

  def index
    @planets = Planet.all
    render :text => @planets.to_plist
  end

end
