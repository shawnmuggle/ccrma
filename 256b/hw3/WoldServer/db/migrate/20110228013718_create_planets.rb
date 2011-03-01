class CreatePlanets < ActiveRecord::Migration
  def self.up
    create_table :planets do |t|

      t.float :r
      t.float :g
      t.float :b
      
      t.string :texture

      t.timestamps
    end
  end

  def self.down
    drop_table :planets
  end
end